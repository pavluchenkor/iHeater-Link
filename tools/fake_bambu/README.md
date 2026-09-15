# fake_bambu — заглушка Bambu Lab MQTT

Локальный TLS-MQTT broker + publisher, эмулирующий принтер Bambu Lab
для проверки iHeater Link Bambu integration без реального принтера.

## Что это

Bambu Lab принтер изнутри запускает MQTT-broker на TLS-порту 8883
с user=`bblp` / pass=`<LAN access code>`. iHeater Link подключается
к нему как клиент, подписывается на `device/{serial}/report` и читает
`chamber_target` / `ams.tray[].tray_type`.

Эта связка `mosquitto + publisher.py` слушает ESP-коннекту и шлёт
в `device/{serial}/report` цикл состояний печати по 15 секунд:

```
PREPARE (PLA) → RUNNING ams (PLA) → RUNNING vt_tray (PETG)
              → RUNNING ams_ht (PA-CF) → FINISH (пусто) → ...
```

Две модели принтера — переключаются `FAKE_BAMBU_MODEL`:

| Модель          | Камера                                     | Откуда берётся целевая температура |
| --------------- | ------------------------------------------ | ---------------------------------- |
| `p1s` (дефолт)  | нет датчика, `chamber_target` = 0           | тип филамента лотка → `mat_<type>` |
| `x1c`           | есть, шлёт `chamber_target`/`chamber_temper` | от принтера                        |
| `h2d`           | есть, новая прошивка: `device.ctc.info.temp` | от принтера                        |

`h2d` проверяет разбор новой упаковки: обе температуры лежат в одном числе —
младшее слово текущая, старшее целевая (`3276846` = цель 50, сейчас 46).
Полей `chamber_temper`/`chamber_target` в этом режиме нет вовсе, как и у
настоящего принтера с новой прошивкой.

В обоих режимах во время печати идут `mc_percent`, `mc_remaining_time`
(в минутах, как у настоящего принтера) и слои `layer_num`/`total_layer_num`.
На `pushall` заглушка отвечает полным снимком, как настоящий принтер.

## Зависимости

```bash
brew install mosquitto
pip3 install --user paho-mqtt
```

## Запуск

В трёх терминалах:

```bash
# 1. Cert (один раз).
cd tools/fake_bambu
./gen_cert.sh

# 2. MQTT-broker (foreground).
mosquitto -c mosquitto.conf

# 3. Publisher (foreground).
python3 publisher.py
```

Параметры publisher.py настраиваются env-переменными:

| Переменная           | Дефолт           | Описание                                    |
| -------------------- | ---------------- | ------------------------------------------- |
| `FAKE_BAMBU_HOST`    | `127.0.0.1`      | broker host для самого publisher            |
| `FAKE_BAMBU_PORT`    | `8883`           | broker port                                 |
| `FAKE_BAMBU_SERIAL`  | `FAKE_BAMBU_001` | serial — должен совпасть с настройкой ESP   |
| `FAKE_BAMBU_LAN`     | `12345678`       | LAN access code                             |
| `FAKE_BAMBU_TRAY`    | `PETG`           | тип филамента в vt_tray (PLA / PETG / ABS)  |
| `FAKE_BAMBU_MODEL`   | `p1s`            | `p1s` / `x1c` / `h2d` — см. таблицу выше    |
| `FAKE_BAMBU_CHAMBER_TARGET` | `50`      | целевая температура камеры для `x1c`, °C    |
| `FAKE_BAMBU_CERT`    | `./cert.pem`     | путь к CA cert                              |

## Настройка интеграции в портале

iHeater Link → Settings → Bambu:

| Поле              | Значение            |
| ----------------- | ------------------- |
| ip                | `<IP вашего мака>`  |
| serial            | `FAKE_BAMBU_001`    |
| lan access code   | `12345678`          |
| enabled           | on                  |

IP мака:

```bash
ipconfig getifaddr en0
```

## Ожидаемый Serial-вывод на ESP

```
[INFO ] BAMBU: configure: ip=192.168.0.171 serial=FAKE_BAMBU_001
[INFO ] BAMBU: connect attempt: 192.168.0.171:8883 (user=bblp)
[INFO ] BAMBU: connected
[INFO ] BAMBU: subscribed to device/FAKE_BAMBU_001/report
[HEATER] Bambu: chamber_target=70 → output=ON
[RMT→status] mode=Drying target=70.0
```

## Диагностика

- Подключение не происходит → mosquitto падает в логе (cert не нашёл, порт занят).
- Publisher теряет соединение → проверьте, что `tls_insecure_set(True)` в скрипте.
- Сертификат под вашу сеть подгонять не нужно: и ESP (`setInsecure()`), и
  publisher (`cert_reqs=CERT_NONE`) личность сервера не проверяют, поэтому
  IP в `subjectAltName` ни на что не влияет. Настоящий принтер Bambu ведёт
  себя так же — его сертификат выписан на заводе и к вашей сети отношения
  не имеет, подлинность подтверждает LAN access code. Запускайте
  `./gen_cert.sh` как есть.
- Хотите проверить tray_type lookup в menu (mat_petg/pla/abs) — задайте
  `FAKE_BAMBU_TRAY=PLA python3 publisher.py` и убедитесь что в меню
  устройства `mat_pla` стоит ненулевое значение.
- Принтер с камерой проверяется так: `FAKE_BAMBU_MODEL=x1c python3 publisher.py`.
  Целевая температура тогда приходит от принтера, а не из типа филамента —
  в логе ESP видно `chamber_target=50`, и меню `mat_*` в выборе не участвует.
- Прогресс печати (`progressPercent` в статусе устройства) растёт от шага к
  шагу и обнуляется на FINISH; оставшееся время идёт в минутах, iHeater
  переводит его в секунды.
