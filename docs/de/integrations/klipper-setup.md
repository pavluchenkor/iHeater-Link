# Klipper-Konfiguration für iHeater Link

## Wofür ist das notwendig

Diese Funktion ist für Drucker auf Klipper mit geschlossenem System gedacht: Creality, Qidi, Flashforge und andere moderne Modelle, bei denen der Benutzer die iHeater-Firmware nicht selbst kompilieren und installieren kann, um direkt mit Klipper zu arbeiten.

Üblicherweise sind jedoch Klipper-Konfigurationsdateien und benutzerdefinierte G-Code-Makros verfügbar. Daher verwendet iHeater Link einen einfacheren Weg: Er verbindet sich mit demselben Wi-Fi-Netzwerk wie der Drucker, liest die Variablen des benutzerdefinierten Klipper-Makros und übermittelt die Zieltemperatur an den iHeater-Controller.

Auf der Druckerseite müssen nur wenige G-Code-Makros hinzugefügt werden. Diese akzeptieren die Standard-Chambertamperatur-Befehle `M141` und `M191` und speichern die Zieltemperatur in `VIRTUAL_CHAMBER`.

Das resultierende Schema:

```text
Slicer / G-code -> M141 S50 -> Klipper macro VIRTUAL_CHAMBER.target=50
                                      |
                                      v
Drucker im lokalen Netzwerk <- Wi-Fi <- iHeater Link -> Signalleitung -> iHeater
```

Der Benutzer benötigt keinen Root-Zugriff und muss nicht in die interne Firmware des Druckers eingreifen. Es genügt, Zugriff auf die benutzerdefinierten Klipper-Makros zu haben.

## Was wird erreicht

```text
M141 S50 -> target = 50 -> iHeater Link schaltet Heizung ein
M141 S0  -> target = 0  -> iHeater Link schaltet Heizung aus
```

Viele moderne Drucker haben bereits einen Temperatursensor in der Kammer. Falls dieser Sensor vom Hersteller vorgesehen ist und in der Klipper-Konfiguration sichtbar ist, kann er verwendet werden, um die tatsächliche Temperatur an das Portal und iHeater Link zu übermitteln. Falls kein Sensor vorhanden ist, kann iHeater Link die Heizung dennoch nach der Zieltemperatur steuern.

## 1. Fügen Sie die Makro-Datei hinzu

Erstellen Sie eine Datei `virtual_chamber.cfg` in der Klipper-Konfiguration und beziehen Sie sie von `printer.cfg` ein:

```ini
[include virtual_chamber.cfg]
```

Inhalt von `virtual_chamber.cfg`:

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

Nach dem Speichern starten Sie Klipper neu oder führen Sie `RESTART` aus.

## 2. Optional: Verbinden Sie den Kammertemperatursensor

Öffnen Sie die Druckerkonfiguration und prüfen Sie, ob ein Objekt vorhanden ist, das einem Kammertemperatursensor ähnelt. Bei verschiedenen Herstellern und Konfigurationen kann es unterschiedliche Namen haben, zum Beispiel:

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

Falls ein solches Objekt vorhanden ist, können Sie die tatsächliche Temperatur an iHeater Link übermitteln. Fügen Sie den folgenden Block zu `virtual_chamber.cfg` hinzu und ersetzen Sie `heater_generic chamber` durch den Namen des Objekts aus Ihrer Konfiguration:

```ini
[delayed_gcode UPDATE_VIRTUAL_CHAMBER_TEMP]
initial_duration: 1.0
gcode:
  {{ "{%" }} set t = printer["heater_generic chamber"].temperature|float {{ "%}" }}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=temperature VALUE={t}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=has_sensor VALUE=1
  UPDATE_DELAYED_GCODE ID=UPDATE_VIRTUAL_CHAMBER_TEMP DURATION=2.0
```

Wenn Sie z.B. einen Sensor als `[temperature_sensor enclosure]` beschrieben haben, sollte die Temperaturlese-Zeile auf `printer["temperature_sensor enclosure"].temperature` verweisen.

Falls kein Sensor vorhanden ist oder Sie unsicher sind, überspringen Sie diesen Schritt. Für die Heizungssteuerung reicht die `target`-Variable aus, die von den Makros `M141` und `M191` übermittelt wird.

## 3. Aktivieren Sie die Klipper-Integration in iHeater Link

Öffnen Sie das iHeater Link-Gerät im Portal und klicken Sie im Block **Device Info** auf **MOONRAKER**. In der Benutzeroberfläche wird dieser Name für Klipper-Drucker verwendet.

![Auswahl von Moonraker in Device Info](../../img/iheater-link-settings-01.png)

Klicken Sie anschließend auf das Zahnrad-Symbol in der Gerätekarte, öffnen Sie die Verbindungseinstellungen und aktivieren Sie **MOONRAKER**.

![Geräteeinstellungen](../../img/iheater-link-settings-03.png)

![Aktivierung von Moonraker in den Geräteeinstellungen](../../img/iheater-link-settings-04.png)

Kehren Sie zu den **MOONRAKER**-Einstellungen zurück, geben Sie die IP-Adresse des Druckers ein und speichern Sie die Einstellungen.

![Rückkehr zu den Moonraker-Einstellungen](../../img/iheater-link-settings-05.png)

![Moonraker-Einstellungen](../../img/iheater-link-settings-02.png)

Üblicherweise reichen folgende Parameter aus:

- Host: IP-Adresse des Druckers im lokalen Netzwerk;
- Port: `7125`;
- API key: lassen Sie es leer, wenn der Drucker keinen Schlüssel benötigt;
- Use SSL (wss): deaktiviert für normale lokale Verbindung;
- Poll interval: `1000`.

Nach dem Speichern beginnt iHeater Link, `VIRTUAL_CHAMBER.target` aus Klipper zu lesen und es an iHeater zu übermitteln.

## 4. Manuelle Steuerung vom Portal

Sie können die Heizung auch ohne Slicer einschalten: Geben Sie die Kammertemperatur in der Gerätekarte ein und klicken Sie auf **START**. Im Zeitfeld können Sie die Heizungsdauer in Minuten angeben. Wenn Sie die Zeit auf `0` lassen, arbeitet iHeater unbegrenzt, bis Sie **STOP** drücken oder einen Ausschaltbefehl senden.

![Manueller Start von iHeater](../../img/iheater-link-settings-03.png)

## 5. Überprüfen Sie die Makro-Funktion

Führen Sie in der Klipper-Konsole Folgendes aus:

```gcode
M141 S50
```

iHeater Link sollte `target=50` erhalten und die iHeater-Heizung einschalten.

Führen Sie dann Folgendes aus:

```gcode
M141 S0
```

Das Target wird auf `0` gesetzt und iHeater Link schaltet die Heizung aus.

## 6. Konfigurieren Sie den Slicer

Der Slicer muss nichts über `VIRTUAL_CHAMBER` wissen. Er sollte Standard-Kammertemperatur-Befehle senden:

- `M141 S{T}` — Kammertemperatur einstellen ohne zu warten;
- `M191 S{T}` — Kammertemperatur einstellen und warten.

Die Klipper-Makros werden diese Befehle abfangen und den Wert in `VIRTUAL_CHAMBER.target` schreiben.

### OrcaSlicer / Bambu Studio

Die Kammertemperatur wird im Filament-Profil eingestellt:

```text
Filament Settings -> Temperatures -> Chamber temperature
```

Zum Beispiel:

- ABS / ASA: `40-50 °C`;
- PLA: `0 °C`.

Überprüfen Sie den Anfang des G-Code. Dort sollte eine Zeile wie diese erscheinen:

```gcode
M141 S45
```

### PrusaSlicer / SuperSlicer

Falls das Profil ein Kammertemperatur-Feld hat, verwenden Sie dieses. Falls nicht, fügen Sie den Befehl manuell in den Start G-Code ein:

```gcode
M141 S45 ; chamber temperature for this filament
```

## 7. Schalten Sie die Heizung am Ende des Drucks unbedingt aus

Nach dem Druck wird target nicht automatisch zurückgesetzt. Fügen Sie in den End G-Code Folgendes ein:

```gcode
CLEAR_VIRTUAL_CHAMBER
```

oder:

```gcode
M141 S0
```

Dies setzt `VIRTUAL_CHAMBER.target` auf `0`, woraufhin iHeater Link iHeater ausschaltet.
