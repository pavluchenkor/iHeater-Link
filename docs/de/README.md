# iHeater Link — Kurzanleitung

iHeater Link — Verbindungsmodul für den Controller **iHeater** mit der Firmware [iheater_revХ_Х_pulse](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases). Dies ist eine Platine auf ESP32-C3 / ESP32-S3, die:

- Sich mit W-LAN verbindet und iHeater mit [portal.idryer.org](https://portal.idryer.org/) verbindet.
- Die Zieltemperatur der Kammer vom Drucker über
  Integrationen **Moonraker (Klipper)**, **Bambu Lab (LAN)** oder
  **Home Assistant** erhält.
- Die Zieltemperatur in ein Impulssignal umwandelt und es an einen GPIO-Pin des iHeater-Controllers sendet.

Die Kontrolle von iHeater erfolgt „über Draht": ein Signalpin ESP → Signaleingang iHeater. W-LAN und Integrationen sind Aufgabe von Link, Heizen und Sicherheit sind Aufgabe von iHeater.

Die drahtgebundene Verbindung schränkt die Platzierung von Link nicht ein. Die ESP-Platine kann außerhalb der Thermokammer untergebracht werden. Dies vermeidet:

- Überhitzung des Chips und der Peripherie bei Betrieb der Kammer bei 60+ °C;
- thermisches Einfrieren des Funkteils und Abbruch der W-LAN-Sitzung bei längerer Heizung;
- beschleunigte Verschlechterung.

Innerhalb der Kammer bleibt nur der iHeater, der für den Betrieb bei hohen Temperaturen ausgelegt ist. Die Länge des Signalkabels zum ESP ist nur durch angemessene Belastung der Leitung begrenzt (Dutzende von Zentimetern — ohne Vorbehalte).

## Unterstützte Platinen

| Platine                      |       |
| ---------------------------- | ----- |
| ESP32-C3 Super Mini          | `✅`  |
| ESP32-C3 DevKitM-1           | `✅`  |
| Seeed XIAO ESP32-S3          | `✅`  |
| Waveshare ESP32-S3-Zero      | `✅`  |

Jede andere Platine auf Basis von ESP32-C3 oder ESP32-S3 kann verwendet werden, wenn
ein freier GPIO für den Signalausgang verfügbar ist. Überprüfen Sie das
Pinout-Diagramm des Herstellers.

## Schaltschema für die Verdrahtung

!!! warning "Trennen Sie niemals Kabel unter Spannung an oder ab."

Die Stromversorgung erfolgt über USB-C zum ESP. Der ESP wiederum versorgt den iHeater-Controller über eine 5-V-Leitung. Dies ist die einfachste Variante. Bei Bedarf kann die Stromversorgung von iHeater anders organisiert werden — die Verbindung mit Link hängt nicht vom Stromversorgungsschema ab.

![Anschluss des ESP32-C3 Super Mini an iHeater](../img/iHeaterLink.png)

Verbindungen (für alle unterstützten Platinen):

| ESP        | iHeater          | Zweck                 |
| ---------- | ---------------- | --------------------- |
| `5V`       | `5V`             | Stromversorgung des Controllers |
| `GND`      | `GND`            | Masse                 |
| `GPIO3`    | Signaleingang    | Impulssollwert        |

### Pinout der Platinen

ESP32-C3 Super Mini:

![Pinout des ESP32-C3 Super Mini](../img/ESP32-C3-Super-Mini-pinout-low.jpg)

Waveshare ESP32-S3-Zero:

![Pinout Waveshare ESP32-S3-Zero](../img/ESP32-C3-ZERO-Waveshare-pinout-low.jpg)

## Firmware über Web-Flasher

Der Web-Flasher befindet sich unter [install.idryer.org](https://install.idryer.org/).

1. Schließen Sie Link über USB an den Computer an.
2. Öffnen Sie [install.idryer.org](https://install.idryer.org/) und wählen Sie das Gerät **iHeater Link** aus.
3. Wählen Sie die Platinen-Variante aus.
4. Klicken Sie auf **Connect**, wählen Sie den seriellen Port aus (normalerweise `USB JTAG/serial`
   oder `CH340`). Wenn das Gerät nicht erkannt wird, halten Sie die `BOOT`-Taste
   auf der Platine gedrückt und drücken Sie kurz `RST`.
5. Klicken Sie auf **Install**. Der Flasher schreibt die Firmware.
6. Nach Abschluss des Flashens wird der W-LAN-Setup-Assistent geöffnet.

## W-LAN-Konfiguration

Nach dem Flashen wird der Improv-Assistent automatisch auf dem Serienport geöffnet.

1. Geben Sie die SSID und das Passwort Ihres 2,4-GHz-Netzwerks ein.
2. Warten Sie auf den Status **Connected**. Der Link-Indikator wechselt in
   den Blinkzustand „Atmung" in Blau.

Wenn der Assistent nicht geöffnet wurde, trennen Sie USB und verbinden Sie es erneut über
**Connect** ohne erneutes Flashen.

!!! note "ESP32 unterstützt nur 2,4 GHz. 5-GHz-Netzwerke funktionieren nicht."

## Bindung an Portal

1. Klicken Sie auf der Flasher-Seite auf **Verbinden und Anspruch ausführen**.
2. Der Befehl `START_CLAIM` wird an das Gerät gesendet. Nach einigen Sekunden
   erscheint eine PIN auf der Seite. Die PIN ist etwa 5 Minuten gültig.
3. Öffnen Sie [portal.idryer.org](https://portal.idryer.org/) → **Gerät hinzufügen** → geben Sie die PIN ein.
4. Nach erfolgreicher Bindung wird das Gerät in der Onlineliste angezeigt.

Wenn Sie `CLAIM_ALREADY:DEVICE_…` erhalten — das Gerät ist bereits mit diesem oder einem anderen Konto verbunden. Löschen Sie in diesem Fall das Gerät im Portal und wiederholen Sie die Bindung.

## Verbindung mit iHeater

1. Trennen Sie die Stromversorgung des Controllers.
2. Verbinden Sie den ESP mit iHeater nach dem obigen Schema: `5V`, `GND`, `GPIO3` →
   Signaleingang iHeater.
3. Verbinden Sie USB mit dem ESP. Der Controller wird über die 5-V-Leitung mit Strom versorgt.

Nach dem Start stellt Link eine Verbindung zum Portal her, aktiviert
die ausgewählte Integration und beginnt, die Zieltemperatur der
Kammer an iHeater zu übertragen.

## Was sollte das Ergebnis sein

- LED 1 leuchtet ständig, LED 3 blinkt kurz 1 Mal pro Sekunde, dies signalisiert die Verbindung zwischen iHeater Link und iHeater.
- Bei Verbindungsverlust blinken alle LEDs mit einer Frequenz von 1 Hz.
- Andere Fehler sind mit iHeater Link verbunden und wiederholen die Indikation der Standalone-Firmware.

## Diagnose

Im Geräte-Menü gibt es einen Punkt **DIAGNOSTICS → DIAG LOG**. Wenn dieser aktiviert ist, wird im Serialport jede Sekunde ein detaillierter Bericht ausgegeben: W-LAN-Status, MQTT, aktive Integration, aktuelles Sollwert, Fehler der Connectors.

Weitere Informationen zur Diagnose finden Sie in der Dokumentation der idryer-core-Bibliothek, die im GitHub-Repository des Projekts verfügbar ist.
