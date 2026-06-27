# Configuración de Klipper para iHeater Link

## Para qué sirve

Esta funcionalidad está diseñada para impresoras Klipper en sistemas cerrados: Creality, Qidi, Flashforge y otros modelos modernos, donde el usuario no puede compilar e instalar el firmware iHeater para trabajar directamente con Klipper.

Los archivos de configuración estándar de Klipper y las macros G-code personalizadas suelen estar disponibles. Por lo tanto, iHeater Link utiliza un enfoque más simple: se conecta a la misma red Wi-Fi que la impresora, lee las variables de las macros personalizadas de Klipper y transmite la temperatura objetivo al controlador iHeater.

En el lado de la impresora solo es necesario agregar algunas macros G-code. Aceptan comandos de temperatura de cámara estándar `M141` y `M191` y almacenan la temperatura objetivo en `VIRTUAL_CHAMBER`.

El esquema resultante es:

```text
Cortadora / G-code -> M141 S50 -> Macro de Klipper VIRTUAL_CHAMBER.target=50
                                      |
                                      v
Impresora en red local <- Wi-Fi <- iHeater Link -> línea de señal -> iHeater
```

El usuario no necesita obtener acceso root ni interferir con el firmware interno de la impresora. Solo es necesario tener acceso a las macros personalizadas de Klipper.

## Qué se obtiene

```text
M141 S50 -> target = 50 -> iHeater Link activa el calentamiento
M141 S0  -> target = 0  -> iHeater Link desactiva el calentamiento
```

Muchas impresoras modernas ya tienen un sensor de temperatura dentro de la cámara. Si el fabricante incluye tal sensor y es visible en la configuración de Klipper, se puede usar para transmitir la temperatura real al portal e iHeater Link. Si no hay sensor, iHeater Link aún podrá controlar el calentamiento por la temperatura objetivo.

## 1. Agregue el archivo de macros

Cree el archivo `virtual_chamber.cfg` en la configuración de Klipper e inclúyalo desde `printer.cfg`:

```ini
[include virtual_chamber.cfg]
```

Contenido de `virtual_chamber.cfg`:

```ini
[gcode_macro VIRTUAL_CHAMBER]
variable_target: 0
variable_temperature: -1
variable_has_sensor: 0
gcode:

[gcode_macro M141]
gcode:
  {{ "{%" }} set t = params.S|default(0)|float {{ "%}" }}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=target VALUE={t}

[gcode_macro M191]
gcode:
  {{ "{%" }} set t = params.S|default(0)|float {{ "%}" }}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=target VALUE={t}

[gcode_macro CLEAR_VIRTUAL_CHAMBER]
gcode:
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=target VALUE=0
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=temperature VALUE=-1
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=has_sensor VALUE=0
```

Después de guardar, reinicie Klipper o ejecute `RESTART`.

## 2. Opcionalmente, conecte el sensor de cámara

Abra la configuración de la impresora y verifique si contiene un objeto que se parezca a un sensor de temperatura de cámara. Diferentes fabricantes y compilaciones pueden llamarlo de diferentes formas, por ejemplo:

```ini
[temperature_sensor chamber]
```

```ini
[temperature_sensor enclosure]
```

```ini
[temperature_sensor chamber_temp]
```

```ini
[heater_generic chamber]
```

Si existe tal objeto, puede transmitir a iHeater Link la temperatura real. Agregue el bloque a continuación en `virtual_chamber.cfg` y reemplace `heater_generic chamber` con el nombre del objeto de su configuración:

```ini
[delayed_gcode UPDATE_VIRTUAL_CHAMBER_TEMP]
initial_duration: 1.0
gcode:
  {{ "{%" }} set t = printer["heater_generic chamber"].temperature|float {{ "%}" }}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=temperature VALUE={t}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=has_sensor VALUE=1
  UPDATE_DELAYED_GCODE ID=UPDATE_VIRTUAL_CHAMBER_TEMP DURATION=2.0
```

Por ejemplo, si su sensor se describe como `[temperature_sensor enclosure]`, la línea de lectura de temperatura debe referirse a `printer["temperature_sensor enclosure"].temperature`.

Si no tiene sensor o no está seguro, omita este paso. Para controlar el calentamiento es suficiente `target`, que transmiten las macros `M141` y `M191`.

## 3. Active la integración de Klipper en iHeater Link

En el portal, abra el dispositivo iHeater Link y haga clic en **MOONRAKER** en el bloque **Device Info**. En la interfaz, este nombre se utiliza para impresoras Klipper.

![Seleccionar Moonraker en Device Info](../../img/iheater-link-settings-01.png)

Luego, haga clic en el icono de engranaje en la tarjeta del dispositivo, abra la configuración de conexiones y active **MOONRAKER**.

![Configuración del dispositivo](../../img/iheater-link-settings-03.png)

![Activar Moonraker en la configuración del dispositivo](../../img/iheater-link-settings-04.png)

Vuelva a la configuración de **MOONRAKER**, ingrese la dirección IP de la impresora y guarde la configuración.

![Volver a la configuración de Moonraker](../../img/iheater-link-settings-05.png)

![Configuración de Moonraker](../../img/iheater-link-settings-02.png)

Normalmente, estos parámetros son suficientes:

- Host: dirección IP de la impresora en la red local;
- Port: `7125`;
- API key: dejar vacío si la impresora no lo requiere;
- Use SSL (wss): deshabilitado para la conexión local normal;
- Poll interval: `1000`.

Después de guardar, iHeater Link comenzará a leer `VIRTUAL_CHAMBER.target` de Klipper y lo transmitirá a iHeater.

## 4. Control manual desde el portal

El calentamiento se puede activar sin el cortador: establezca la temperatura de la cámara en la tarjeta del dispositivo y haga clic en **START**. En el campo de tiempo puede especificar la duración del calentamiento en minutos. Si deja el tiempo en `0`, iHeater funcionará sin límite de tiempo hasta que haga clic en **STOP** o envíe un comando de apagado.

![Inicio manual de iHeater](../../img/iheater-link-settings-03.png)

## 5. Verifique el funcionamiento de las macros

En la consola de Klipper, ejecute:

```gcode
M141 S50
```

iHeater Link debe recibir `target=50` y activar el calentamiento de iHeater.

Luego ejecute:

```gcode
M141 S0
```

Target se convertirá en `0`, e iHeater Link desactivará el calentamiento.

## 6. Configure el cortador

El cortador no necesita conocer `VIRTUAL_CHAMBER`. Debe enviar comandos de temperatura de cámara estándar:

- `M141 S{T}` — establece la temperatura de la cámara sin esperar;
- `M191 S{T}` — establece la temperatura de la cámara con espera.

Las macros de Klipper interceptarán estos comandos y escribirán el valor en `VIRTUAL_CHAMBER.target`.

### OrcaSlicer / Bambu Studio

La temperatura de la cámara se establece en el perfil de filamento:

```text
Filament Settings -> Temperatures -> Chamber temperature
```

Por ejemplo:

- ABS / ASA: `40-50 °C`;
- PLA: `0 °C`.

Verifique el inicio de G-code. Debe contener una línea como:

```gcode
M141 S45
```

### PrusaSlicer / SuperSlicer

Si el perfil tiene un campo de temperatura de cámara, úselo. Si no hay campo, agregue el comando manualmente en Start G-code:

```gcode
M141 S45 ; chamber temperature for this filament
```

## 7. Asegúrese de desactivar el calentamiento al final de la impresión

Al final de la impresión, target no se reinicia automáticamente. Agregue en End G-code:

```gcode
CLEAR_VIRTUAL_CHAMBER
```

o:

```gcode
M141 S0
```

Esto restablecerá `VIRTUAL_CHAMBER.target` a `0`, después de lo cual iHeater Link desactivará iHeater.
