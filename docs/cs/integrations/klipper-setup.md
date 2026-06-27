# Nastavení Klipper pro iHeater Link

## K čemu je to potřeba

Tato funkce je určena pro tiskárny s Klipperem v uzavřených systémech: Creality, Qidi, Flashforge a dalších moderních modelech, kde uživatel nemůže sám sestavit a nainstalovat firmware iHeater pro přímou práci s Klipperem.

Běžné konfigurační soubory Klipperu a uživatelská makra G-code jsou obvykle dostupná. Proto iHeater Link používá jednodušší cestu: připojí se ke stejné Wi-Fi síti jako tiskárna, čte proměnné uživatelských maker Klipperu a předává cílovou teplotu kontroléru iHeater.

Na straně tiskárny stačí přidat jen několik maker G-code. Přijímají standardní příkazy pro teplotu komory `M141` a `M191` a ukládají cílovou teplotu do `VIRTUAL_CHAMBER`.

Výsledné schéma:

```text
Slicovač / G-code -> M141 S50 -> Klipper macro VIRTUAL_CHAMBER.target=50
                                      |
                                      v
Tiskárna v lokální síti <- Wi-Fi <- iHeater Link -> signální linka -> iHeater
```

Uživatel nemusí získávat root přístup nebo zasahovat do interního firmware tiskárny. Stačí mít přístup k uživatelským makrům Klipperu.

## Co se bude dít

```text
M141 S50 -> target = 50 -> iHeater Link zapne ohřev
M141 S0  -> target = 0  -> iHeater Link vypne ohřev
```

Mnoho moderních tiskáren již má teplotní senzor uvnitř komory. Pokud je takový senzor navržen výrobcem a viditelný v konfiguraci Klipperu, lze jej použít k odesílání skutečné teploty na portál a iHeater Link. Pokud senzor není k dispozici, iHeater Link stále bude moci řídit ohřev podle cílové teploty.

## 1. Přidejte soubor maker

Vytvořte soubor `virtual_chamber.cfg` v konfiguraci Klipperu a připojte jej z `printer.cfg`:

```ini
[include virtual_chamber.cfg]
```

Obsah `virtual_chamber.cfg`:

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

Po uložení restartujte Klipper nebo spusťte `RESTART`.

## 2. Volitelně připojte senzor komory

Otevřete konfiguraci tiskárny a zkontrolujte, zda v ní existuje objekt podobný teplotnímu senzoru komory. U různých výrobců a sestav se může jmenovat různě, například:

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

Pokud takový objekt existuje, můžete odesílat iHeater Link skutečnou teplotu. Přidejte do `virtual_chamber.cfg` níže uvedený blok a nahraďte `heater_generic chamber` názvem objektu ze své konfigurace:

```ini
[delayed_gcode UPDATE_VIRTUAL_CHAMBER_TEMP]
initial_duration: 1.0
gcode:
  {{ "{%" }} set t = printer["heater_generic chamber"].temperature|float {{ "%}" }}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=temperature VALUE={t}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=has_sensor VALUE=1
  UPDATE_DELAYED_GCODE ID=UPDATE_VIRTUAL_CHAMBER_TEMP DURATION=2.0
```

Pokud máte například senzor popsaný jako `[temperature_sensor enclosure]`, měl by řádek pro čtení teploty odkazovat na `printer["temperature_sensor enclosure"].temperature`.

Pokud senzor není k dispozici nebo si nejste jisti, tento krok přeskočte. Pro řízení ohřevu stačí `target`, který poskytují makra `M141` a `M191`.

## 3. Povolte integraci Klipperu v iHeater Link

Na portálu otevřete zařízení iHeater Link a klikněte na **MOONRAKER** v sekci **Device Info**. V rozhraní se toto jméno používá pro tiskárny Klipperu.

![Výběr Moonraker v Device Info](../../img/iheater-link-settings-01.png)

Poté klikněte na ikonu ozubeného kola na kartě zařízení, otevřete nastavení připojení a povolte **MOONRAKER**.

![Nastavení zařízení](../../img/iheater-link-settings-03.png)

![Povolení Moonraker v nastavení zařízení](../../img/iheater-link-settings-04.png)

Vraťte se do nastavení **MOONRAKER**, zadejte IP adresu tiskárny a uložte nastavení.

![Návrat do nastavení Moonraker](../../img/iheater-link-settings-05.png)

![Nastavení Moonraker](../../img/iheater-link-settings-02.png)

Obvykle stačí tyto parametry:

- Host: IP adresa tiskárny v lokální síti;
- Port: `7125`;
- API key: nechte prázdné, pokud je tiskárna nevyžaduje;
- Use SSL (wss): vypnuto pro běžné místní připojení;
- Poll interval: `1000`.

Po uložení začne iHeater Link číst `VIRTUAL_CHAMBER.target` z Klipperu a předávat jej do iHeater.

## 4. Ruční řízení z portálu

Ohřev lze zapínat i bez slicovače: nastavte teplotu komory na kartě zařízení a klikněte na **START**. V poli času můžete zadat dobu ohřevu v minutách. Pokud necháte čas `0`, bude iHeater běžet bez časového omezení, dokud neklinkete na **STOP** nebo neodešlete příkaz k vypnutí.

![Ruční spuštění iHeater](../../img/iheater-link-settings-03.png)

## 5. Zkontrolujte funkci maker

V konzoli Klipperu spusťte:

```gcode
M141 S50
```

iHeater Link by měl obdržet `target=50` a zapnout ohřev iHeater.

Poté spusťte:

```gcode
M141 S0
```

Target se změní na `0` a iHeater Link vypne ohřev.

## 6. Nastavte slicovač

Slicovač nemusí znát `VIRTUAL_CHAMBER`. Měl by odesílat standardní příkazy pro teplotu komory:

- `M141 S{T}` — nastavit teplotu komory bez čekání;
- `M191 S{T}` — nastavit teplotu komory se čekáním.

Makra Klipperu zachytí tyto příkazy a zapíší hodnotu do `VIRTUAL_CHAMBER.target`.

### OrcaSlicer / Bambu Studio

Teplota komory se nastavuje v profilu filamentu:

```text
Filament Settings -> Temperatures -> Chamber temperature
```

Například:

- ABS / ASA: `40-50 °C`;
- PLA: `0 °C`.

Zkontrolujte začátek G-code. Měl by se tam zobrazit řádek jako:

```gcode
M141 S45
```

### PrusaSlicer / SuperSlicer

Pokud profil obsahuje pole teploty komory, použijte jej. Pokud pole není k dispozici, přidejte příkaz ručně do Start G-code:

```gcode
M141 S45 ; chamber temperature for this filament
```

## 7. Vždy vypněte ohřev na konci tisku

Po skončení tisku se target automaticky neresetuje. Přidejte do End G-code:

```gcode
CLEAR_VIRTUAL_CHAMBER
```

nebo:

```gcode
M141 S0
```

Tímto se resetuje `VIRTUAL_CHAMBER.target` na `0`, poté iHeater Link vypne iHeater.
