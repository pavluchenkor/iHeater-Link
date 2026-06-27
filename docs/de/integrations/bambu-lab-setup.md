# Bambu Lab für iHeater Link einrichten

## Wofür ist das erforderlich

iHeater Link kann iHeater während des Druckens auf Bambu Lab Druckern automatisch steuern. Im Gegensatz zum Klipper-Szenario müssen Sie keine Makros hinzufügen oder G-Code auf dem Drucker ändern. Link verbindet sich über das lokale Netzwerk mit dem Drucker und liest den Druckstatus und das aktive Filament aus.

Bei den aktuellen Bambu Lab Druckern ist es nicht möglich, die Kammerwärmeabgabe zuverlässig als Steuersignal für iHeater zu verwenden. Stattdessen kann Link erkennen, welches Filament sich derzeit im aktiven Tray befindet und ob der Drucker mit dem Druckvorgang beginnt oder bereits druckt. Basierend auf dem Filamenttyp wählt die Firmware die erforderliche Kammertemperatur aus der Materialtabelle aus und aktiviert iHeater automatisch.

Falls ein erforderlicher Filamenttyp nicht in der Materialliste der Firmware vorhanden ist, kontaktieren Sie den Firmware-Autor: Der Typ kann in zukünftigen Versionen hinzugefügt werden.

## Erwartetes Ergebnis

```text
Bambu beginnt Druckvorbereitungen oder Drucken -> Link erkennt aktives Filament -> wählt Temperatur aus Materialtabelle -> iHeater wird aktiviert
```

Wenn der Druckvorgang beendet wird oder das aktive Szenario keine Erwärmung mehr erfordert, deaktiviert Link iHeater.

## 1. Geräteeinstellungen öffnen

Öffnen Sie im Portal die iHeater Link Gerätekarte und klicken Sie auf das Zahnradsymbol.

![Geräteeinstellungen öffnen](../../img/iheater-link-settings-bambu-02.png)

## 2. Bambu-Verbindung aktivieren

Öffnen Sie in den Geräteeinstellungen den Block **CONNECTIONS** und aktivieren Sie **BAMBU**. Die übrigen Verbindungen können deaktiviert bleiben, wenn sie nicht verwendet werden.

![Bambu in Geräteeinstellungen aktivieren](../../img/iheater-link-settings-bambu-03.png)

## 3. Bambu Lab auf der Gerätseite auswählen

Kehren Sie zur Gerätseite zurück und klicken Sie auf **BAMBU LAB** im Block **Device Info**. Die Schaltfläche wird aktiv.

![Bambu Lab auswählen](../../img/iheater-link-settings-bambu-04.png)

## 4. Verbindungsparameter eingeben

Aktivieren Sie in den **Bambu Lab** Einstellungen die Integration und geben Sie die Verbindungsparameter ein.

![Bambu Lab Einstellungen](../../img/iheater-link-settings-bambu-05.png)

In der Regel sind folgende Felder erforderlich:

- Printer IP: IP-Adresse des Druckers im lokalen Netzwerk;
- Printer serial: Seriennummer des Druckers;
- LAN access code: LAN-Modus Zugriffscode;
- Auto-apply on tag detect: Aktiviert, wenn Link die Temperatur automatisch basierend auf dem erkannten Filament anwenden soll;
- Default AMS und Default tray: Diese Werte können auf ihren Standardwerten belassen werden, wenn Sie nicht erzwingen müssen, einen bestimmten AMS oder Tray auszuwählen.

Der Drucker und iHeater Link müssen sich im selben lokalen Netzwerk befinden. Der LAN access code und die Seriennummer können in den Bambu Lab Druckereinstellungen gefunden werden.

## 5. Materialtemperaturen konfigurieren

Öffnen Sie den Block **MATERIALS** in den Geräteeinstellungen. Sie können für jeden Filamenttyp eine separate Kammertemperatur einstellen.

![Materialtemperaturen](../../img/iheater-link-settings-001-materials.png)

Die Firmware enthält bereits eine große Anzahl von Filamenttypen. Wenn der Drucker mit der Druckvorbereitung oder dem Druckvorgang beginnt, überprüft Link das aktive Tray, bestimmt den Filamenttyp und ruft die Temperatur aus dieser Tabelle ab. Sie können beispielsweise für PLA eine niedrige Temperatur einstellen oder das Heizen deaktivieren, für ABS und ASA eine höhere Temperatur einstellen.

Wenn das aktive Filament nicht in der Tabelle vorhanden ist oder die eingestellte Temperatur nicht geeignet ist, bearbeiten Sie den Wert in **MATERIALS** und speichern Sie die Einstellungen.

## 6. Funktionalität überprüfen

Starten Sie einen Druckvorgang auf Bambu Lab mit einem Filament, für das in **MATERIALS** eine Temperatur definiert ist. Wenn der Drucker zur Druckvorbereitung oder zum Druckvorgang übergeht, sollte iHeater Link automatisch die Temperatur für das aktive Filament anwenden und iHeater aktivieren.

Wenn die Erwärmung nicht aktiviert wird, überprüfen Sie folgende Punkte:

- Ist die **BAMBU** Verbindung in den Geräteeinstellungen aktiviert;
- Ist **BAMBU LAB** im Block **Device Info** ausgewählt;
- Sind IP, Seriennummer und LAN access code korrekt angegeben;
- Erkennt der Drucker das aktive Tray und den Filamenttyp;
- Ist eine Temperatur für diesen Filamenttyp in **MATERIALS** definiert.
