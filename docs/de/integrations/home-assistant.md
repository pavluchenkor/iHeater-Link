# iHeater Link mit Home Assistant verbinden

iHeater Link veröffentlicht ein Gerät in Home Assistant über MQTT Discovery: HA erstellt automatisch eine Karte mit echten Sensoren und Steuerelementen (Zieltemperatur, Dauer, Modus IDLE/DRYING/STORAGE).

!!! note
    Das Gerät wird **nicht** unter `Settings → Devices & services → Discovered` angezeigt. iHeater Link verwendet MQTT Discovery, nicht UPnP/zeroconf. In Home Assistant muss die **MQTT**-Integration **bereits hinzugefügt** sein und auf Ihren Broker verweisen.

## Voraussetzungen

1. MQTT-Broker (z. B. Mosquitto Add-on) läuft in HA oder ist im Netzwerk verfügbar.
2. In HA ist die **MQTT**-Integration mit dem Broker hinzugefügt und konfiguriert.
3. iHeater Link hat über das Portal den Befehl `link_integration {type:"ha"}` erhalten und eine Verbindung zu demselben Broker hergestellt.

## Schritt 1. Einstellungen öffnen

Klicken Sie unten im Seitenmenü von Home Assistant auf **Settings**.

![Einstellungen im Seitenmenü](../../img/HA-integration-01.png)

## Schritt 2. Zu Devices & services navigieren

Wählen Sie **Devices & services** in der Liste der Einstellungsabschnitte.

![Devices & services](../../img/HA-integration-02.png)

## Schritt 3. MQTT-Integration öffnen

Suchen Sie in der Liste der Integrationen die Karte **MQTT**. Darunter finden Sie einen Zähler der verbundenen Geräte.

![MQTT in der Liste der Integrationen](../../img/HA-integration-03.png)

## Schritt 4. iDryer-Gerät finden

Erweitern Sie auf der Integrationenseite im Abschnitt **Services** den Broker-Knoten (`127.0.0.1` oder die Adresse Ihres Brokers). Darunter sind iDryer-Geräte mit ihren Seriennummern aufgeführt, z. B. `DEVICE_*`.

![MQTT-Geräte](../../img/HA-integration-04.png)

Klicken Sie auf das gewünschte Gerät.

## Schritt 5. Verwaltung und Status

Auf der Gerätoseite gibt es zwei Blöcke:

- **Controls** — Steuerelemente:
  - `iDryer U1 duration` — Dauer in Minuten
  - `iDryer U1 mode control` — Modus (`IDLE` / `DRYING` / `STORAGE`)
  - `iDryer U1 target temp` — Zieltemperatur (Schieberegler)
- **Sensors** — aktuelle Werte. Die Zusammensetzung hängt vom Gerätetyp ab (`Config` definiert die veröffentlichten Sensoren):
  - iHeater Link: `heater_power`, `mode`, `alerts`
  - Storage Link: das gleiche plus `temperature`, `humidity`

![Gerätoseite in HA](../../img/HA-integration-05.png)

Um das Heizen zu starten:

1. Stellen Sie die Zieltemperatur mit dem Schieberegler ein.
2. Stellen Sie die Dauer ein.
3. Wählen Sie den Modus `DRYING` oder `STORAGE` in der Auswahl.

Zum Stoppen wechseln Sie die Auswahl auf `IDLE`.

!!! note
    Die Werte `target temp` und `duration` werden zunächst auf dem Gerät als "ausstehend" gespeichert. Der eigentliche Start erfolgt bei Auswahl des Modus. Dies ermöglicht das Festlegen von Parametern in beliebiger Reihenfolge und das Starten mit einer Aktion.

## Was unter der Haube passiert

- **Discovery** (Erstellen einer Entity in der HA UI mit den richtigen Symbolen) — wird automatisch beim Verbinden mit dem HA-Broker veröffentlicht. Die Zusammensetzung wird durch die Flags `Config.hasXxx` bestimmt — fehlende Sensoren erscheinen nicht als Phantome.
- **State** (aktuelle Werte) — wird alle 5 Sekunden in HA-Topics veröffentlicht, parallel zur Veröffentlichung im Portal.
- **Commands** (`set_temp` / `set_duration` / `set_mode`) — gehen von HA → MQTT-Broker → Gerät → werden in `Request` gesammelt und durchlaufen denselben Weg wie Portal-Befehle. Keine HA-spezifischen Zweige im Produktcode.

## Fehlerbehebung

| Symptom | Was zu überprüfen ist |
|---------|----------------------|
| Gerät wird nicht in HA angezeigt | Überprüfen Sie auf dem Gerät im Portal: `Home Assistant → Enabled: yes`. Das Feld `ha.state` in `integrations/status` sollte `online` sein. |
| Discovery ist veröffentlicht, aber die Karte ist leer | Warten Sie 5–10 Sekunden nach der ersten Verbindung. Wenn Werte nicht angezeigt werden — überprüfen Sie, ob der MQTT-Broker keine Retained-Nachrichten verliert. |
| Steuerschaltflächen reagieren nicht | Überprüfen Sie das `command_topic` aus Discovery — das Topic sollte mit `idryer/{serial}/U1/set_mode` usw. übereinstimmen. |
| Phantom-Sensoren mit dem Wert Unknown | Alte Retained Discovery von der vorherigen Firmwareversion. Nach dem Update warten Sie entweder auf den nächsten Discovery-Veröffentlichungszyklus oder löschen Sie Retained: `mosquitto_pub -t 'homeassistant/.../config' -n -r`. |
