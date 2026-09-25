# Home Assistant

iHeater Link meldet sich in Home Assistant über **MQTT Discovery** an: HA erstellt die Entitäten selbst — Temperatur, Heizleistung, Felder für den Start und Schaltflächen. Das Portal wird dafür nicht benötigt, alles läuft über Ihren MQTT-Broker.

Nachfolgend: die Integration einschalten, prüfen und ein fertiges Karten-Layout, damit das Gerät aufgeräumt aussieht und nicht wie eine Liste von Entitäten.

![Karte von iHeater Link in Home Assistant](../../img/iheater-ha-card.png)
*Kammertemperatur, Heizleistung und Start des Heizens in einem Block.*

!!! note
    Das Gerät **erscheint nicht** unter `Settings → Devices & services → Discovered`: Das ist MQTT Discovery, nicht UPnP/zeroconf. Die Integration **MQTT** muss in Home Assistant bereits hinzugefügt sein.

## Was benötigt wird

1. Ein MQTT-Broker: das Add-on **Mosquitto broker** in Home Assistant oder ein beliebiger Broker in Ihrem Netzwerk.
2. Die in Home Assistant hinzugefügte Integration **MQTT**, die auf diesen Broker zeigt.
3. iHeater Link im Netzwerk und `Online` auf dem Portal.

!!! info "iHeater Link — das Kommunikationsmodul für den iHeater-Controller; flashen Sie den Controller mit der Firmware [iheater_revX_X_pulse](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases)."

## Schritt 1. Die Integration am Gerät einschalten

Öffnen Sie das Gerät auf [portal.idryer.org](https://portal.idryer.org/) und suchen Sie den Block **Integrationen** → **Home Assistant**.

| Feld | Was einzutragen ist |
|---|---|
| Host | die Adresse des Brokers in Ihrem Netzwerk, zum Beispiel `192.168.1.27` |
| Port | der Port des Brokers, üblicherweise `1883` |
| Username / Password | die Zugangsdaten des Brokers, falls er sie verlangt |
| Discovery prefix | `homeassistant`, sofern Sie ihn in den HA-Einstellungen nicht geändert haben |
| Aktiviert | das Häkchen — sonst verbindet sich das Gerät nicht mit dem Broker |

Die Einstellungen gehen über das lokale Netzwerk direkt an das Gerät — das Portal speichert sie nicht. Home Assistant wird über einen eigenen Schalter eingeschaltet und stört die Druckerintegrationen nicht: Bambu Lab und Moonraker werden getrennt ausgewählt, und es arbeitet immer nur eine von beiden.

![Das Fenster Home Assistant im Block „Integrationen“ auf dem Portal](../../img/iheater-ha-portal-integration.png)
*Die Adresse des Brokers, der Port und das Kennzeichen „Aktiviert“ — mehr braucht das Gerät nicht.*

## Schritt 2. Das Gerät in Home Assistant finden

Klicken Sie unten im Seitenmenü auf **Settings**.

![Settings im Seitenmenü](../../img/HA-integration-01.png)

Wählen Sie **Devices & services**.

![Devices & services](../../img/HA-integration-02.png)

Suchen Sie die Karte **MQTT**. Unter dem Namen steht der Zähler der verbundenen Geräte.

![MQTT in der Liste der Integrationen](../../img/HA-integration-03.png)

Klappen Sie im Abschnitt **Services** den Knoten des Brokers auf. Die iDryer-Geräte erscheinen unter Seriennummern der Form `DEVICE_*`.

![MQTT-Geräte](../../img/HA-integration-04.png)

Öffnen Sie das Gerät: HA zeigt bereits die Messwerte und die Bedienelemente.

![Die Geräteseite in HA](../../img/HA-integration-05.png)

## Schritt 3. Die Karte zusammenstellen

HA ordnet die Entitäten selbst an, und es entsteht eine lange Liste. Das fertige Layout stellt die Messwerte nach oben und den Start des Heizens in einen eigenen Block.

1. `Settings` → `Dashboards` → **Add dashboard** → ein leeres Dashboard, öffnen Sie es.
2. Rechte obere Ecke → der Stift (**Edit**) → das Menü „⋮“ → **Raw configuration editor**.
3. Fügen Sie den nachfolgenden Inhalt ein und speichern Sie.

Das Layout ist für ein Dashboard des Typs `sections` ausgelegt.

```yaml
title: iDryer
views:
- title: Devices
  path: devices
  type: sections
  max_columns: 4
  sections:
  - type: grid
    background: true
    cards:
    - type: heading
      heading: iHeater Link
      heading_style: title
      icon: mdi:radiator
      badges:
      - type: entity
        entity: sensor.iheater_link_mode
        show_icon: false
        show_state: true
        color: primary
    - type: tile
      entity: sensor.iheater_link_temperature
      name: Temperature
      visibility:
      - condition: state
        entity: sensor.iheater_link_temperature
        state_not:
        - unknown
        - unavailable
    - type: tile
      entity: sensor.iheater_link_heater_power
      name: Heater power
    - type: heading
      heading: Heat
      heading_style: subtitle
    - type: tile
      entity: number.iheater_link_heat_temperature
      name: Temperature
      features:
      - type: numeric-input
        style: buttons
      features_position: bottom
    - type: tile
      entity: number.iheater_link_heat_duration
      name: Duration
      features:
      - type: numeric-input
        style: buttons
      features_position: bottom
    - type: tile
      entity: button.iheater_link_heat
      name: Start heating
      icon: mdi:play
      hide_state: true
      tap_action: &id001
        action: perform-action
        perform_action: button.press
        target:
          entity_id: button.iheater_link_heat
      icon_tap_action: *id001
    - type: tile
      entity: button.iheater_link_stop
      name: Stop
      icon: mdi:stop
      hide_state: true
      tap_action: &id002
        action: perform-action
        perform_action: button.press
        target:
          entity_id: button.iheater_link_stop
      icon_tap_action: *id002
```

![Raw configuration editor mit dem eingefügten Layout](../../img/iheater-ha-raw-editor.png)
*Dasselbe Layout im Konfigurationseditor des Dashboards.*

Die Reihenfolge des Starts ist dieselbe wie in der App: Zuerst werden Temperatur und Dauer festgelegt, danach wird **Heizen starten** gedrückt. Die Schaltfläche **Stopp** schaltet das Heizen aus.

## Wenn die Namen der Entitäten nicht übereinstimmen

Das Layout ist für Standardbezeichner der Form `sensor.iheater_link_temperature` ausgelegt. Zeigt die Karte „Entity not found“, sehen Sie Ihre eigenen nach: `Settings` → `Devices & services` → **MQTT** → Ihr Gerät → die Liste der Entitäten — und ersetzen Sie das Präfix im Layout durch Ihres.

## Was unter der Haube passiert

- Den Bestand der Entitäten meldet das Gerät selbst — aus der Beschreibung seiner Karte: Messwerte, Parameterfelder und Aktionsschaltflächen. Was das Gerät nicht hat, erscheint auch nicht in Home Assistant.
- Die Werte werden zusammen mit der gewöhnlichen Telemetrie in MQTT veröffentlicht; HA erhält sie in Echtzeit.
- Ein Tastendruck in HA kommt beim Gerät als dieselbe Aktion an wie aus dem Portal oder der App — eine gesonderte Logik „für Home Assistant“ gibt es in der Firmware nicht.

## Diagnose

| Symptom | Was zu prüfen ist |
|---|---|
| Das Gerät ist nicht in HA erschienen | Auf dem Portal ist bei der Integration Home Assistant das Häkchen „Aktiviert“ gesetzt, Adresse und Port des Brokers stimmen. Das Gerät muss `Online` sein. |
| Es ist erschienen, aber die Werte sind `Unknown` | Warten Sie einen Telemetriezyklus ab. Bleibt es weiterhin leer — der Broker speichert keine Retained-Nachrichten oder das Gerät hat sich nicht mit ihm verbunden. |
| Es gibt keine Kammertemperatur | Der Sensor ist nicht an den iHeater-Controller angeschlossen: Ohne ihn veröffentlicht das Gerät diesen Wert nicht. |
| Die Schaltflächen reagieren nicht | Prüfen Sie, dass der Broker das Veröffentlichen in die Topics `idryer/#` erlaubt und dass im Log des Geräts keine Autorisierungsfehler stehen. |
| Phantom-Entitäten mit dem Wert `Unknown` | Es sind Retained-Nachrichten der früheren Firmware übrig geblieben. Löschen Sie sie: `mosquitto_pub -h <Broker> -t 'homeassistant/<...>/config' -n -r`. |
| Zusammen mit Home Assistant sind Bambu oder Moonraker verschwunden | Home Assistant hat damit nichts zu tun — er wird getrennt eingeschaltet. Prüfen Sie die Auswahl der Druckerintegration: Es arbeitet immer nur eine von beiden. |
