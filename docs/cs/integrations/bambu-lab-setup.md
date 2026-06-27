# Nastavení Bambu Lab pro iHeater Link

## Proč je to potřebné

iHeater Link může automaticky řídit iHeater během tisku na tiskárnách Bambu Lab. Na rozdíl od scénáře Klipper není nutné přidávat makra nebo měnit G-code tiskárny. Link se připojuje k tiskárně přes místní síť a čte stav tisku a aktivní filament.

Na současných tiskárnách Bambu Lab nelze spolehlivě použít přenos teploty komory jako příkaz pro řízení iHeater. Link však může určit, který filament je nyní instalován v aktivní zásobě a že tiskárna zahájila přípravu tisku nebo již tiskne. Na základě typu filamentu firmware zvolí potřebnou teplotu komory z tabulky materiálů a automaticky zapne iHeater.

Pokud požadovaný typ filamentu není v seznamu materiálů firmware, napište autorovi firmware: typ lze přidat v budoucích verzích.

## Co se stane

```text
Bambu zahajuje přípravu nebo tisk -> Link vidí aktivní filament -> vybere teplotu z tabulky materiálů -> iHeater se zapne
```

Když se tisk skončí nebo aktivní scénář už nevyžaduje topení, Link vypne iHeater.

## 1. Otevřete nastavení zařízení

V portálu otevřete kartu iHeater Link a klikněte na ikonu ozubeného kola.

![Otevření nastavení zařízení](../../img/iheater-link-settings-bambu-02.png)

## 2. Povolte připojení Bambu

V nastavení zařízení otevřete blok **CONNECTIONS** a povolte **BAMBU**. Ostatní připojení můžete ponechat vypnutá, pokud je nepoužíváte.

![Povolení Bambu v nastavení zařízení](../../img/iheater-link-settings-bambu-03.png)

## 3. Vyberte Bambu Lab na stránce zařízení

Vraťte se na stránku zařízení a klikněte na **BAMBU LAB** v bloku **Device Info**. Tlačítko se aktivuje.

![Výběr Bambu Lab](../../img/iheater-link-settings-bambu-04.png)

## 4. Zadejte parametry připojení

V nastavení **Bambu Lab** povolte integraci a vyplňte parametry připojení.

![Nastavení Bambu Lab](../../img/iheater-link-settings-bambu-05.png)

Obvykle jsou potřebná tato pole:

- Printer IP: IP adresa tiskárny v místní síti;
- Printer serial: sériové číslo tiskárny;
- LAN access code: přístupový kód režimu LAN;
- Auto-apply on tag detect: povoleno, pokud má Link automaticky aplikovat teplotu podle rozpoznaného filamentu;
- Default AMS a Default tray: můžete ponechat výchozí hodnoty, pokud nepotřebujete vynutit konkrétní AMS nebo zásobník.

Tiskárna a iHeater Link musí být ve stejné místní síti. LAN access code a sériové číslo jsou převzata z nastavení tiskárny Bambu Lab.

## 5. Nakonfigurujte teploty materiálů

V nastavení zařízení otevřete blok **MATERIALS**. Pro každý typ filamentu můžete nastavit vlastní teplotu komory.

![Teploty materiálů](../../img/iheater-link-settings-001-materials.png)

Firmware již obsahuje širokou sadu typů filamentů. Když tiskárna zahájí přípravu tisku nebo samotný tisk, Link zkontroluje aktivní zásobník, určí typ filamentu a vezme teplotu z této tabulky. Například pro PLA můžete nastavit nízkou teplotu nebo tisk vypnout, pro ABS a ASA nastavit vyšší teplotu.

Pokud aktivní filament není v tabulce nalezen nebo je pro něj nastavena nevhodná teplota, upravte hodnotu v **MATERIALS** a uložte nastavení.

## 6. Zkontrolujte funkčnost

Spusťte tisk na Bambu Lab s filamentem, pro který je v **MATERIALS** nastavena teplota. Když se tiskárna přepne na přípravu nebo tisk, iHeater Link by měl automaticky aplikovat teplotu pro aktivní filament a zapnout iHeater.

Pokud se topení nezapíná, zkontrolujte:

- je povoleno připojení **BAMBU** v nastavení zařízení;
- je vybrán **BAMBU LAB** v bloku **Device Info**;
- jsou správně zadány IP, serial a LAN access code;
- vidí tiskárna aktivní zásobník a typ filamentu;
- je v **MATERIALS** nastavena teplota pro tento typ filamentu.
