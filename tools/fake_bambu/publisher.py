#!/usr/bin/env python3
"""
Fake Bambu publisher — шлёт report-payloads в device/{SERIAL}/report.

Две модели принтера, переключаются FAKE_BAMBU_MODEL:

  p1s (по умолчанию) — без датчика камеры (P1S/A1). chamber_target всегда 0,
      целевую температуру iHeater берёт из типа филамента активного трея
      через menu.mat_<type>.
  x1c — с датчиком камеры. Шлёт chamber_target/chamber_temper, температура
      камеры приходит от принтера, тип филамента в выборе не участвует.

В обоих режимах во время печати идут mc_percent, mc_remaining_time (минуты,
как у настоящего принтера) и слои layer_num/total_layer_num.

Лестница по 15 сек:
  PREPARE (PLA) → RUNNING ams (PLA) → RUNNING vt_tray (PETG)
                → RUNNING ams_ht (PA-CF) → FINISH (пусто) → ...

Состояния gcode_state:
  PREPARE — принтер готовится (homing/прогрев), tray уже выбран
  RUNNING — печать идёт
  FINISH  — печать завершена, трей выгружен (tray_now="255")

ESP-side auto_heat включает нагрев при PREPARE и RUNNING, выключает
при FINISH (см. iHeater-link/src/heater/auto_heat.cpp::bambuShouldHeat).

Зависимости: pip3 install --user paho-mqtt
"""
import json, ssl, sys, time, logging, os

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s",
    datefmt="%H:%M:%S",
)
log = logging.getLogger("fake-bambu")

try:
    import paho.mqtt.client as mqtt
except ImportError:
    print("pip3 install --user paho-mqtt", file=sys.stderr)
    sys.exit(1)

BROKER         = os.environ.get("FAKE_BAMBU_HOST", "127.0.0.1")
PORT           = int(os.environ.get("FAKE_BAMBU_PORT", "8883"))
SERIAL         = os.environ.get("FAKE_BAMBU_SERIAL", "FAKE_BAMBU_001")
LAN_CODE       = os.environ.get("FAKE_BAMBU_LAN", "12345678")
CERT_PATH      = os.environ.get("FAKE_BAMBU_CERT",
                                os.path.join(os.path.dirname(__file__), "cert.pem"))
# p1s — без датчика камеры (chamber_target = 0), x1c — с датчиком,
# h2d — с датчиком и новой прошивкой (температуры в device.ctc, см. ниже).
MODEL          = os.environ.get("FAKE_BAMBU_MODEL", "p1s").lower()
HAS_CHAMBER    = MODEL in ("x1c", "x1", "x1e", "h2d")
# Новая прошивка не шлёт chamber_temper/chamber_target, а пакует пару
# «текущая/целевая» в одно число внутри device.ctc.info.temp.
PACKED_TEMPS   = MODEL == "h2d"
# Целевая температура камеры для моделей с датчиком, °C.
CHAMBER_TARGET = float(os.environ.get("FAKE_BAMBU_CHAMBER_TARGET", "50"))
TOTAL_LAYERS   = 120
# Лестница: каждый источник филамента → свой тип, по 15 сек.
#
#   ("ams",      "PLA")    — обычный AMS slot 0 (tray_now="0")
#   ("vt_tray",  "PETG")   — внешний держатель катушки (tray_now="254")
#   ("ams_ht",   "PA-CF")  — AMS HT (tray_now="128"), полиамиды
#   ("finish",   None)     — печать завершена, трей выгружен (tray_now="255")
PATTERN = [
    ("prepare", "PLA"),
    ("ams",     "PLA"),
    ("vt_tray", "PETG"),
    ("ams_ht",  "PA-CF"),
    ("finish",  None),
]
STEP_SECONDS = 15

REPORT_TOPIC  = f"device/{SERIAL}/report"
REQUEST_TOPIC = f"device/{SERIAL}/request"

# Текущее состояние лестницы — нужно для ответа на pushall.
current_index = 0


def print_progress(source: str):
    """Прогресс печати для текущего шага: (mc_percent, оставшиеся минуты, слой).

    Настоящий принтер шлёт mc_remaining_time в минутах — iHeater переводит их
    в секунды сам. Перед печатью и после неё прогресса нет.
    """
    if source == "prepare":
        return 0, 40, 0
    if source == "finish":
        return 100, 0, TOTAL_LAYERS
    # Шаги печати идут подряд, поэтому процент растёт от цикла к циклу.
    running_step = max(0, current_index) % 12
    percent = min(99, 10 + running_step * 8)
    return percent, max(1, 40 - running_step * 3), int(TOTAL_LAYERS * percent / 100)


def progress_fields(source: str):
    percent, minutes, layer = print_progress(source)
    return {
        "mc_percent":        percent,
        "mc_remaining_time": minutes,
        "layer_num":         layer,
        "total_layer_num":   TOTAL_LAYERS,
    }


def chamber_fields(source: str):
    """Поля камеры. У модели без датчика их нет вовсе — как у настоящего P1S.

    Новая прошивка (h2d) шлёт вместо chamber_* блок device.ctc.info.temp, где
    в одном числе упакованы обе температуры: младшее слово — текущая, старшее
    — целевая. Так же устроен и device.bed.
    """
    if not HAS_CHAMBER:
        return {}
    heating = source not in ("finish",)
    target = CHAMBER_TARGET if heating else 0.0
    # Текущая ползёт к цели, но не дотягивает — камера греется медленно.
    current = 24.0 if source == "prepare" else (target - 4.0 if heating else 28.0)

    if PACKED_TEMPS:
        return {
            "device": {
                "ctc": {"info": {"temp": (int(target) << 16) | int(current)}, "state": 0},
                "bed": {"info": {"temp": (60 << 16) | 58}, "state": 2},
            }
        }
    return {"chamber_target": target, "chamber_temper": round(current, 1)}


def make_payload(source: str, tray_type):
    """source: 'prepare' | 'ams' | 'vt_tray' | 'ams_ht' | 'finish'."""
    if source == "prepare":
        # Принтер готовится к печати: tray уже выбран, прогрев/homing.
        # ESP должен включить нагрев камеры (auto_heat: PREPARE → heat).
        return {
            "print": {
                "command":     "push_status",
                "gcode_state": "PREPARE",
                **progress_fields(source),
                **chamber_fields(source),
                "ams": {
                    "tray_now": "0",
                    "ams": [{
                        "id": "0",
                        "tray": [
                            {"id": "0", "tray_type": tray_type, "tray_info_idx": "GFA00"},
                            {"id": "1"},
                            {"id": "2"},
                            {"id": "3"},
                        ],
                    }],
                },
            }
        }

    if source == "ams":
        # tray_now="0" → ams_index = 0>>2 = 0, tray_index = 0&3 = 0
        return {
            "print": {
                "command":     "push_status",
                "gcode_state": "RUNNING",
                **progress_fields(source),
                **chamber_fields(source),
                "ams": {
                    "tray_now": "0",
                    "ams": [{
                        "id": "0",
                        "tray": [
                            {"id": "0", "tray_type": tray_type, "tray_info_idx": "GFA00"},
                            {"id": "1"},
                            {"id": "2"},
                            {"id": "3"},
                        ],
                    }],
                },
            }
        }

    if source == "vt_tray":
        # tray_now="254" → внешняя катушка, tray_type из vt_tray
        return {
            "print": {
                "command":     "push_status",
                "gcode_state": "RUNNING",
                **progress_fields(source),
                **chamber_fields(source),
                "ams": {
                    "tray_now": "254",
                    "vt_tray":  {"tray_type": tray_type, "tray_info_idx": "GFB99"},
                    "ams":      [],
                },
            }
        }

    if source == "ams_ht":
        # tray_now>=80 → AMS HT, ams_index = tray_now (128), tray_index = 0
        return {
            "print": {
                "command":     "push_status",
                "gcode_state": "RUNNING",
                **progress_fields(source),
                **chamber_fields(source),
                "ams": {
                    "tray_now": "128",
                    "ams": [{
                        "id": "128",
                        "tray": [
                            {"id": "0", "tray_type": tray_type, "tray_info_idx": "GFN05"},
                        ],
                    }],
                },
            }
        }

    # finish
    return {
        "print": {
            "command":     "push_status",
            "gcode_state": "FINISH",
            **progress_fields(source),
            **chamber_fields(source),
            "ams": {"tray_now": "255"},
        }
    }


def main():
    log.info(f"broker={BROKER}:{PORT} serial={SERIAL} lan={LAN_CODE}")
    log.info(f"topic={REPORT_TOPIC}")
    log.info(f"pattern={PATTERN} step={STEP_SECONDS}s")
    log.info(f"model={MODEL} chamber_sensor={'yes' if HAS_CHAMBER else 'no'}"
             + (f" chamber_target={CHAMBER_TARGET}°C" if HAS_CHAMBER else ""))

    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2,
                         client_id=f"fake-bambu-publisher-{os.getpid()}")
    client.username_pw_set("bblp", LAN_CODE)

    if os.path.exists(CERT_PATH):
        client.tls_set(ca_certs=CERT_PATH,
                       tls_version=ssl.PROTOCOL_TLS_CLIENT,
                       cert_reqs=ssl.CERT_NONE)
    else:
        log.warning(f"cert {CERT_PATH} not found — using TLS without CA")
        client.tls_set(tls_version=ssl.PROTOCOL_TLS_CLIENT,
                       cert_reqs=ssl.CERT_NONE)
    client.tls_insecure_set(True)

    def on_connect(c, u, f, rc, p):
        log.info(f"connected rc={rc}")
        # Слушаем команды от ESP — Bambu принтер делает то же.
        c.subscribe(REQUEST_TOPIC, qos=0)
        log.info(f"subscribed to {REQUEST_TOPIC}")

    def on_message(c, u, msg):
        try:
            payload = json.loads(msg.payload.decode("utf-8", "replace"))
        except Exception as e:
            log.warning(f"req parse err: {e}")
            return
        log.info(f"← request: {payload}")
        # Реальный Bambu по pushall шлёт полный initial snapshot.
        # У нас это — текущий шаг лестницы.
        pushing = payload.get("pushing", {})
        if pushing.get("command") == "pushall":
            source, tray_type = PATTERN[current_index % len(PATTERN)]
            snap = json.dumps(make_payload(source, tray_type))
            c.publish(REPORT_TOPIC, snap, qos=0, retain=True)
            log.info(f"→ pushall reply: source={source} tray_type={tray_type or '<empty>'}")

    client.on_connect    = on_connect
    client.on_disconnect = lambda c, u, f, rc, p: log.warning(f"disconnected rc={rc}")
    client.on_message    = on_message

    client.connect(BROKER, PORT, keepalive=60)
    client.loop_start()

    try:
        global current_index
        while True:
            source, tray_type = PATTERN[current_index % len(PATTERN)]
            payload = json.dumps(make_payload(source, tray_type))
            client.publish(REPORT_TOPIC, payload, qos=0, retain=True)
            percent, minutes, layer = print_progress(source)
            log.info(f"→ source={source:8s} tray_type={tray_type or '<empty>':7s} "
                     f"{percent:3d}% {minutes:3d}min layer {layer}/{TOTAL_LAYERS}"
                     + (f" chamber (device.ctc packed)" if PACKED_TEMPS else
                        f" chamber {chamber_fields(source)['chamber_temper']}"
                        f"/{chamber_fields(source)['chamber_target']}°C" if HAS_CHAMBER else ""))
            current_index += 1
            time.sleep(STEP_SECONDS)
    except KeyboardInterrupt:
        pass
    finally:
        client.loop_stop()
        client.disconnect()


if __name__ == "__main__":
    main()
