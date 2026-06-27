# Conectar iHeater Link a Home Assistant

iHeater Link publica el dispositivo en Home Assistant mediante MQTT Discovery: HA crea automáticamente una tarjeta con sensores reales y elementos de control (temperatura objetivo, duración, modo IDLE/DRYING/STORAGE).

!!! note
    El dispositivo **no aparecerá** en `Settings → Devices & services → Discovered`. iHeater Link utiliza MQTT Discovery, no UPnP/zeroconf. En Home Assistant debe estar **ya agregada** la integración **MQTT**, apuntando a tu broker.

## Qué debe estar listo

1. El broker MQTT (por ejemplo complemento Mosquitto) ejecutándose en HA o accesible por red.
2. En HA agregada la integración **MQTT** con el broker configurado.
3. iHeater Link recibió a través del portal el comando `link_integration {type:"ha"}` y estableció conexión con el mismo broker.

## Paso 1. Abrir configuración

En el menú lateral de Home Assistant, en la parte inferior haz clic en **Settings**.

![Settings en el menú lateral](../../img/HA-integration-01.png)

## Paso 2. Ir a Devices & services

En la lista de secciones de configuración selecciona **Devices & services**.

![Devices & services](../../img/HA-integration-02.png)

## Paso 3. Abrir integración MQTT

En la lista de integraciones encuentra la tarjeta **MQTT**. Bajo el nombre — contador de dispositivos conectados.

![MQTT en la lista de integraciones](../../img/HA-integration-03.png)

## Paso 4. Encontrar dispositivo iDryer

En la página de integración, en la sección **Services**, expande el nodo del broker (`127.0.0.1` o la dirección de tu broker). Bajo él se listan los dispositivos iDryer con sus números de serie como `DEVICE_*`.

![Dispositivos MQTT](../../img/HA-integration-04.png)

Haz clic en el dispositivo deseado.

## Paso 5. Control y estado

En la página del dispositivo hay dos bloques:

- **Controls** — elementos de control:
  - `iDryer U1 duration` — duración en minutos
  - `iDryer U1 mode control` — modo (`IDLE` / `DRYING` / `STORAGE`)
  - `iDryer U1 target temp` — temperatura objetivo (deslizador)
- **Sensors** — valores reales. La composición depende del tipo de dispositivo (`Config` determina los sensores publicados):
  - iHeater Link: `heater_power`, `mode`, `alerts`
  - Storage Link: lo mismo más `temperature`, `humidity`

![Página del dispositivo en HA](../../img/HA-integration-05.png)

Para iniciar calefacción:

1. Establece la temperatura objetivo con el deslizador.
2. Establece la duración.
3. Selecciona modo `DRYING` o `STORAGE` en el selector.

Para detener — cambia el selector a `IDLE`.

!!! note
    Los valores `target temp` y `duration` primero se guardan en el dispositivo como «pendientes», el inicio real ocurre al seleccionar el modo. Esto permite establecer parámetros en cualquier orden e iniciar con una acción.

## Qué sucede bajo el capó

- **Discovery** (creación de entity en HA UI con iconos correctos) — se publica automáticamente al conectarse al broker HA. La composición se determina por las banderas `Config.hasXxx` — los sensores faltantes no aparecen como fantasmas.
- **State** (valores actuales) — se publica en tópicos HA cada 5 segundos en paralelo con la publicación en el portal.
- **Comandos** (`set_temp` / `set_duration` / `set_mode`) — van de HA → broker MQTT → dispositivo → se recogen en `Request` y pasan por la misma ruta que los comandos del portal. Sin ramas específicas de HA en el código del producto.

## Diagnóstico

| Síntoma | Qué verificar |
|---------|---------------|
| El dispositivo no aparece en HA | En el dispositivo en el portal — `Home Assistant → Habilitado: sí`. El campo `ha.state` en `integrations/status` debe ser `online`. |
| Discovery se publica pero la tarjeta está vacía | Espera 5–10 segundos después de la primera conexión. Si los valores no aparecen — verifica que el broker MQTT no pierda mensajes retained. |
| Los botones de control no responden | Verifica `command_topic` de Discovery — el tópico debe coincidir con `idryer/{serial}/U1/set_mode` etc. |
| Sensores fantasma con valor Unknown | Viejos retained Discovery de la versión anterior del firmware. Después de actualizar, espera el siguiente ciclo de publicación Discovery o limpia los retained: `mosquitto_pub -t 'homeassistant/.../config' -n -r`. |
