# Připojení iHeater Link k Home Assistant

iHeater Link publikuje zařízení v Home Assistant přes MQTT Discovery: HA automaticky vytvoří kartu se skutečnými senzory a prvky ovládání (cílová teplota, trvání, režim IDLE/DRYING/STORAGE).

!!! note
    Zařízení se **nezobrazí** v `Settings → Devices & services → Discovered`. iHeater Link používá MQTT Discovery, nikoli UPnP/zeroconf. V Home Assistant musí být **již přidána** integrace **MQTT** s nakonfigurovaným brokerem.

## Co musí být připraveno

1. MQTT-broker (například doplněk Mosquitto) spuštěný v HA nebo dostupný v síti.
2. V HA je přidána integrace **MQTT** s nakonfigurovaným brokerem.
3. iHeater Link obdržel příkaz `link_integration {type:"ha"}` prostřednictvím portálu a navázal spojení se stejným brokerem.

## Krok 1. Otevření nastavení

V postranní nabídce Home Assistant klikněte dole na **Settings**.

![Nastavení v postranní nabídce](../../img/HA-integration-01.png)

## Krok 2. Přechod na Devices & services

V seznamu sekcí nastavení vyberte **Devices & services**.

![Devices & services](../../img/HA-integration-02.png)

## Krok 3. Otevření integrace MQTT

V seznamu integrací vyhledejte kartu **MQTT**. Pod názvem je čítač připojených zařízení.

![MQTT v seznamu integrací](../../img/HA-integration-03.png)

## Krok 4. Vyhledání zařízení iDryer

Na stránce integrace v sekci **Services** rozbalte uzel brokeru (`127.0.0.1` nebo adresa vašeho brokeru). Pod ním jsou uvedena zařízení iDryer s jejich sériovými čísly ve formě `DEVICE_*`.

![Zařízení MQTT](../../img/HA-integration-04.png)

Klikněte na požadované zařízení.

## Krok 5. Ovládání a stav

Na stránce zařízení jsou dva bloky:

- **Controls** — prvky ovládání:
  - `iDryer U1 duration` — trvání v minutách
  - `iDryer U1 mode control` — režim (`IDLE` / `DRYING` / `STORAGE`)
  - `iDryer U1 target temp` — cílová teplota (posuvník)
- **Sensors** — skutečné hodnoty. Složení závisí na typu zařízení (`Config` určuje publikované senzory):
  - iHeater Link: `heater_power`, `mode`, `alerts`
  - Storage Link: totéž plus `temperature`, `humidity`

![Stránka zařízení v HA](../../img/HA-integration-05.png)

Spuštění oteplování:

1. Nastavte cílovou teplotu pomocí posuvníku.
2. Nastavte trvání.
3. V selektoru zvolte režim `DRYING` nebo `STORAGE`.

Zastavení — přepněte selektor na `IDLE`.

!!! note
    Hodnoty `target temp` a `duration` se nejdřív uloží na zařízení jako „odložené", skutečný start nastane při výběru režimu. To umožňuje nastavit parametry v libovolném pořadí a spustit jedinou akcí.

## Co se děje pod kapotou

- **Discovery** (vytvoření entity v HA UI se správnými ikonami) — publikuje se automaticky při připojení k HA-brokeru. Složení určují příznaky `Config.hasXxx` — chybějící senzory se neobjeví jako duševní.
- **State** (aktuální hodnoty) — publikuje se v HA-témata každých 5 sekund paralelně s publikací na portál.
- **Příkazy** (`set_temp` / `set_duration` / `set_mode`) — putují z HA → MQTT-broker → zařízení → sbírají se v `Request` a procházejí stejnou cestou jako příkazy portálu. V produktovém kódu nejsou žádné HA-specifické větve.

## Diagnostika

| Příznak | Co zkontrolovat |
|---------|-----------------|
| Zařízení se v HA nezobrazuje | V portálu na zařízení — `Home Assistant → Zapnuto: ano`. Pole `ha.state` v `integrations/status` by mělo být `online`. |
| Discovery je publikován, ale karta je prázdná | Počkejte 5–10 sekund po prvním připojení. Pokud se hodnoty neobjeví — zkontrolujte, že MQTT-broker neztratí zachované zprávy. |
| Tlačítka ovládání nereagují | Zkontrolujte `command_topic` z Discovery — téma by se mělo shodovat s `idryer/{serial}/U1/set_mode` atd. |
| Duševní senzory s hodnotou Unknown | Staré zachované Discovery z předchozí verze firmware. Po aktualizaci buďte trpěliví na další cyklus publikace Discovery, nebo vyčistěte zachované: `mosquitto_pub -t 'homeassistant/.../config' -n -r`. |
