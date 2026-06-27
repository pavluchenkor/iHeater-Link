# iHeater Link — stručný průvodce

iHeater Link — komunikační modul pro řadič **iHeater** s firmware [iheater_revХ_Х_pulse](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases). Jedná se o desku na bázi ESP32-C3 / ESP32-S3, která:

- Připojuje se k Wi-Fi a propojuje iHeater s [portal.idryer.org](https://portal.idryer.org/).
- Přijímá cílovou teplotu komory od tiskárny prostřednictvím
  integrací **Moonraker (Klipper)**, **Bambu Lab (LAN)** nebo
  **Home Assistant**.
- Přeměňuje cílovou teplotu na pulzní signál a přenáší jej řadiči iHeater na jednom GPIO.

Řízení iHeater probíhá "drátově": jeden signální pin ESP → signální vstup iHeater. Wi-Fi a integrace jsou záležitostí Link, ohřev a bezpečnost jsou záležitostí iHeater.

Jednovodičové spojení nekladou žádná omezení na umístění Link. Desku ESP lze umístit mimo tepelnou komoru. To vylučuje:

- přehřátí čipu a periferií při provozu komory na 60+ °C;
- tepelné zástupy radiové sekce a ztrátu Wi-Fi relace při dlouhodobém ohřevu;
- urychlené zhoršení stavu.

Uvnitř komory zůstává pouze iHeater, navržený pro provoz při vysokých teplotách. Délka signálního vodiče k ESP je omezena pouze rozumným zatížením linky (desítky centimetrů - bez výhrad).

## Podporované desky

| Deska                        |       |
| ---------------------------- | ----- |
| ESP32-C3 Super Mini          | `✅`  |
| ESP32-C3 DevKitM-1           | `✅`  |
| Seeed XIAO ESP32-S3          | `✅`  |
| Waveshare ESP32-S3-Zero      | `✅`  |

Jakoukoli další desku na bázi ESP32-C3 nebo ESP32-S3 lze použít, pokud
je dostupný volný GPIO pro signální výstup. Porovnejte s pinoutem
od výrobce.

## Schéma zapojení

!!! warning "Nikdy nepřipojujte a nepřipojujte vodiče při přivedeném napájení."

Napájení je přivedeno na ESP přes USB-C. ESP zase napájí řadič iHeater linkou 5 V. Toto je nejjednodušší možnost. V případě potřeby lze napájení iHeater uspořádat jinak - komunikace s Link nezávisí na schématu napájení.

![Připojení ESP32-C3 Super Mini k iHeater](../img/iHeaterLink.png)

Spojení (pro všechny podporované desky):

| ESP        | iHeater          | Určení                |
| ---------- | ---------------- | --------------------- |
| `5V`       | `5V`             | napájení řadiče       |
| `GND`      | `GND`            | společná zem          |
| `GPIO3`    | signální vstup   | pulzní setpoint       |

### Pinout desek

ESP32-C3 Super Mini:

![Pinout ESP32-C3 Super Mini](../img/ESP32-C3-Super-Mini-pinout-low.jpg)

Waveshare ESP32-S3-Zero:

![Pinout Waveshare ESP32-S3-Zero](../img/ESP32-C3-ZERO-Waveshare-pinout-low.jpg)

## Firmware přes webový flasher

Webový flasher se nachází na [install.idryer.org](https://install.idryer.org/).

1. Připojte Link k USB portu počítače.
2. Otevřete [install.idryer.org](https://install.idryer.org/) a vyberte zařízení **iHeater Link**.
3. Vyberte variantu desky.
4. Klikněte na **Connect**, vyberte sériový port (obvykle `USB JTAG/serial`
   nebo `CH340`). Pokud zařízení není rozpoznáno, stiskněte tlačítko `BOOT`
   na desce a stiskněte `RST`.
5. Klikněte na **Install**. Flasher zapíše firmware.
6. Po dokončení flashing se otevře průvodce nastavením Wi-Fi.

## Nastavení Wi-Fi

Po flashing se v sériovém portu automaticky otevře průvodce Improv.

1. Zadejte SSID a heslo vaší sítě 2,4 GHz.
2. Počkejte na stav **Connected**. Indikátor Link přejde na
   režim "dýchání" modrou barvou.

Pokud se průvodce neotevřel, odpojte USB a připojte jej znovu pomocí
**Connect** bez opakovaného flashing.

!!! note "ESP32 podporuje pouze 2,4 GHz. Sítě 5 GHz nefungují."

## Vazba na portál

1. Na stránce flasheru klikněte na **Připojit a provést Claim**.
2. Zařízení obdrží příkaz `START_CLAIM`. Za několik sekund
   se na stránce zobrazí PIN. PIN platí ~5 minut.
3. Otevřete [portal.idryer.org](https://portal.idryer.org/) → **Přidat
   zařízení** → zadejte PIN.
4. Po úspěšné vazbě se zařízení zobrazí v seznamu online.

Pokud v odpovědi přišlo `CLAIM_ALREADY:DEVICE_…` - zařízení je již vázáno na tento či jiný účet. V takovém případě smažte zařízení v portálu a zopakujte vazbu.

## Připojení k iHeater

1. Vypněte napájení řadiče.
2. Připojte ESP k iHeater podle schématu výše: `5V`, `GND`, `GPIO3` →
   signální vstup iHeater.
3. Připojte napájení k USB ESP. Řadič se napájí linkou 5 V.

Po spuštění Link estabeliší spojení s portálem, aktivuje
vybranou integraci a začne vysílat cílovou teplotu
komory k iHeater.

## Co by mělo vyjít

- LED 1 svítí nepřetržitě, LED 3 krátce bliká 1krát za sekundu, to signalizuje přítomnost spojení iHeater Link - iHeater.
- při ztrátě spojení všechny LED blikají s frekvencí 1 Hz.
- ostatní chyby nejsou spojeny s iHeater Link a opakují indikaci firmware standalone.

## Diagnostika

V menu zařízení je položka **DIAGNOSTICS → DIAG LOG**. Když ji zapnete, je v Serialu jednou za sekundu výsledný podrobný zprávu: stav Wi-Fi, MQTT, aktivní integraci, aktuální target, chyby konektorů.

Podrobnosti o diagnostice jsou v dokumentaci knihovny idryer-core, která je k dispozici v úložišti projektu na GitHub.
