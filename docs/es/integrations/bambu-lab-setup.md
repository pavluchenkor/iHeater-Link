# Configuración de Bambu Lab para iHeater Link

## Para qué sirve

iHeater Link puede gestionar automáticamente iHeater durante la impresión en impresoras Bambu Lab. A diferencia del escenario de Klipper, no es necesario agregar macros ni modificar el G-code de la impresora. Link se conecta a la impresora a través de la red local y lee el estado de impresión y el filamento activo.

En las impresoras Bambu Lab actuales no es posible utilizar de forma fiable la transmisión de temperatura de cámara como comando de control de iHeater. Sin embargo, Link puede determinar qué filamento está instalado en la bandeja activa y si la impresora ha comenzado la preparación o está imprimiendo. Según el tipo de filamento, el firmware selecciona la temperatura de cámara necesaria de la tabla de materiales e iHeater se enciende automáticamente.

Si el tipo de filamento que necesitas no está en la lista de materiales del firmware, escribe al autor del firmware: el tipo se puede agregar en versiones futuras.

## Qué obtendrá

```text
Bambu comienza la preparación o impresión -> Link ve el filamento activo -> selecciona la temperatura de la tabla de materiales -> iHeater se enciende
```

Cuando la impresión finaliza o el escenario activo ya no requiere calefacción, Link apaga iHeater.

## 1. Abra la configuración del dispositivo

En el portal, abra la tarjeta iHeater Link y presione el icono de engranaje.

![Abriendo la configuración del dispositivo](../../img/iheater-link-settings-bambu-02.png)

## 2. Habilite la conexión Bambu

En la configuración del dispositivo, abra el bloque **CONNECTIONS** y habilite **BAMBU**. Las otras conexiones pueden dejarse deshabilitadas si no se utilizan.

![Habilitando Bambu en la configuración del dispositivo](../../img/iheater-link-settings-bambu-03.png)

## 3. Seleccione Bambu Lab en la página del dispositivo

Vuelva a la página del dispositivo y presione **BAMBU LAB** en el bloque **Device Info**. El botón se activará.

![Seleccionando Bambu Lab](../../img/iheater-link-settings-bambu-04.png)

## 4. Ingrese los parámetros de conexión

En la configuración de **Bambu Lab**, habilite la integración y complete los parámetros de conexión.

![Configuración de Bambu Lab](../../img/iheater-link-settings-bambu-05.png)

Típicamente se necesitan estos campos:

- Printer IP: dirección IP de la impresora en la red local;
- Printer serial: número de serie de la impresora;
- LAN access code: código de acceso para el modo LAN;
- Auto-apply on tag detect: habilitado si Link debe aplicar automáticamente la temperatura según el filamento detectado;
- Default AMS y Default tray: puede dejar los valores predeterminados si no necesita seleccionar forzosamente un AMS o bandeja específicos.

La impresora e iHeater Link deben estar en la misma red local. El código de acceso LAN y el número de serie se obtienen de la configuración de la impresora Bambu Lab.

## 5. Configure las temperaturas de los materiales

Abra el bloque **MATERIALS** en la configuración del dispositivo. Para cada tipo de filamento puede establecer su propia temperatura de cámara.

![Temperaturas de los materiales](../../img/iheater-link-settings-001-materials.png)

El firmware ya incluye un amplio conjunto de tipos de filamentos. Cuando la impresora comience la preparación o impresión, Link verificará la bandeja activa, determinará el tipo de filamento y tomará la temperatura de esta tabla. Por ejemplo, para PLA puede dejar una temperatura baja o desactivar la calefacción, para ABS y ASA establecer una temperatura más alta.

Si el filamento activo no se encuentra en la tabla o se ha establecido una temperatura inadecuada, edite el valor en **MATERIALS** y guarde la configuración.

## 6. Verifique el funcionamiento

Inicie una impresión en Bambu Lab con un filamento para el que haya establecido una temperatura en **MATERIALS**. Cuando la impresora pase a la preparación o impresión, iHeater Link debe aplicar automáticamente la temperatura para el filamento activo e iniciar iHeater.

Si la calefacción no se enciende, verifique:

- que la conexión **BAMBU** esté habilitada en la configuración del dispositivo;
- que **BAMBU LAB** esté seleccionado en el bloque **Device Info**;
- que la IP, serial y código de acceso LAN sean correctos;
- que la impresora vea la bandeja activa y el tipo de filamento;
- que se haya establecido una temperatura para este tipo de filamento en **MATERIALS**.
