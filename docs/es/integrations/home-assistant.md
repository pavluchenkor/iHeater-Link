# Home Assistant

iHeater Link se publica en Home Assistant mediante **MQTT Discovery**: HA crea las entidades por sí mismo — temperatura, potencia de calentamiento, campos de inicio y botones. Para esto no hace falta el portal, todo pasa por tu broker MQTT.

A continuación: activación de la integración, comprobación y una disposición de tarjeta lista para usar, para que el dispositivo se vea ordenado y no como una lista de entidades.

![Tarjeta de iHeater Link en Home Assistant](../../img/iheater-ha-card.png)
*Temperatura de la cámara, potencia de calentamiento e inicio del calentamiento en un solo bloque.*

!!! note
    El dispositivo **no aparecerá** en `Settings → Devices & services → Discovered`: esto es MQTT Discovery, no UPnP/zeroconf. La integración **MQTT** en Home Assistant debe estar añadida de antemano.

## Qué se necesita

1. Un broker MQTT: el complemento **Mosquitto broker** en Home Assistant o cualquier broker de tu red.
2. En Home Assistant, la integración **MQTT** añadida y apuntando a ese broker.
3. iHeater Link en la red y `Online` en el portal.

!!! info "iHeater Link es el módulo de comunicación del controlador iHeater; graba en el controlador el firmware [iheater_revX_X_pulse](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases)."

## Paso 1. Activar la integración en el dispositivo

Abre el dispositivo en [portal.idryer.org](https://portal.idryer.org/) y busca el bloque **Integraciones** → **Home Assistant**.

| Campo | Qué escribir |
|---|---|
| Host | dirección del broker en tu red, por ejemplo `192.168.1.27` |
| Port | puerto del broker, normalmente `1883` |
| Username / Password | credenciales del broker, si las requiere |
| Discovery prefix | `homeassistant`, si no lo has cambiado en los ajustes de HA |
| Activado | la casilla — de lo contrario el dispositivo no se conectará al broker |

Los ajustes van directamente al dispositivo por la red local — el portal no los guarda. Home Assistant se activa con su propio interruptor y no estorba a las integraciones de impresora: Bambu Lab y Moonraker se eligen aparte, y a la vez funciona solo una de ellas.

![Ventana de Home Assistant en el bloque «Integraciones» del portal](../../img/iheater-ha-portal-integration.png)
*Dirección del broker, puerto y la marca «Activado» — todo lo que el dispositivo necesita.*

## Paso 2. Encontrar el dispositivo en Home Assistant

En el menú lateral, abajo, pulsa **Settings**.

![Settings en el menú lateral](../../img/HA-integration-01.png)

Selecciona **Devices & services**.

![Devices & services](../../img/HA-integration-02.png)

Busca la tarjeta **MQTT**. Bajo el nombre está el contador de dispositivos conectados.

![MQTT en la lista de integraciones](../../img/HA-integration-03.png)

En la sección **Services** despliega el nodo del broker. Los dispositivos iDryer se ven bajo números de serie del tipo `DEVICE_*`.

![Dispositivos MQTT](../../img/HA-integration-04.png)

Abre el dispositivo: HA ya muestra las lecturas y los elementos de control.

![Página del dispositivo en HA](../../img/HA-integration-05.png)

## Paso 3. Montar la tarjeta

HA dispone las entidades por su cuenta y sale una lista larga. La disposición lista para usar pone las lecturas arriba y el inicio del calentamiento en un bloque aparte.

1. `Settings` → `Dashboards` → **Add dashboard** → un dashboard vacío, ábrelo.
2. Esquina superior derecha → el lápiz (**Edit**) → menú «⋮» → **Raw configuration editor**.
3. Pega el contenido de abajo y guarda.

La disposición está pensada para un dashboard de tipo `sections`.

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

![Raw configuration editor con la disposición pegada](../../img/iheater-ha-raw-editor.png)
*La misma disposición en el editor de configuración del dashboard.*

El orden de arranque es el mismo que en la aplicación: primero se fijan la temperatura y la duración, después se pulsa **Iniciar calentamiento**. El botón **Parar** apaga el calentamiento.

## Si los nombres de las entidades no coinciden

La disposición está pensada para identificadores estándar del tipo `sensor.iheater_link_temperature`. Si la tarjeta muestra «Entity not found», mira los tuyos: `Settings` → `Devices & services` → **MQTT** → tu dispositivo → lista de entidades, — y sustituye el prefijo de la disposición por el tuyo.

## Qué ocurre bajo el capó

- El dispositivo declara por sí mismo el conjunto de entidades — a partir de la descripción de su propia tarjeta: lecturas, campos de parámetros y botones de acción. Lo que el dispositivo no tiene, no aparece en Home Assistant.
- Los valores se publican en MQTT junto con la telemetría habitual; HA los recibe en tiempo real.
- La pulsación de un botón en HA llega al dispositivo como la misma acción que desde el portal o la aplicación — en el firmware no hay una lógica aparte «para Home Assistant».

## Diagnóstico

| Síntoma | Qué comprobar |
|---|---|
| El dispositivo no aparece en HA | En el portal, la integración Home Assistant tiene marcada la casilla «Activado» y la dirección y el puerto del broker son correctos. El dispositivo debe estar `Online`. |
| Aparece, pero los valores son `Unknown` | Espera un ciclo de telemetría. Si sigue vacío — el broker no guarda mensajes retained o el dispositivo no se ha conectado a él. |
| No hay temperatura de la cámara | El sensor no está conectado al controlador iHeater: sin él el dispositivo no publica esa magnitud. |
| Los botones no funcionan | Comprueba que el broker permita la publicación en los tópicos `idryer/#` y que en el registro del dispositivo no haya errores de autorización. |
| Entidades fantasma con valor `Unknown` | Quedan mensajes retained del firmware anterior. Límpialos: `mosquitto_pub -h <broker> -t 'homeassistant/<...>/config' -n -r`. |
| Junto con Home Assistant desapareció Bambu o Moonraker | Home Assistant no tiene nada que ver — se activa aparte. Comprueba la elección de la integración de impresora: funciona solo una de ellas. |
