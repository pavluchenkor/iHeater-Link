# 為 iHeater Link 設定 Klipper

## 用途

此功能適用於執行 Klipper 的封閉系統 3D 列印機：Creality、Qidi、Flashforge 以及其他現代型號，其中使用者無法編譯和安裝 iHeater 韌體以直接與 Klipper 協作。

通常，標準 Klipper 組態檔和自訂 G 碼巨集可供存取。因此，iHeater Link 採用更簡單的方法：它連接到與列印機相同的 Wi-Fi 網路、讀取自訂 Klipper 巨集變數，並將目標溫度傳遞給 iHeater 控制器。

在列印機一側，您只需新增幾個 G 碼巨集。它們接受標準的室溫命令 `M141` 和 `M191`，並將目標溫度儲存在 `VIRTUAL_CHAMBER` 中。

最終的架構：

```text
切片軟體 / G 碼 -> M141 S50 -> Klipper 巨集 VIRTUAL_CHAMBER.target=50
                                      |
                                      v
區域網路中的列印機 <- Wi-Fi <- iHeater Link -> 訊號線 -> iHeater
```

使用者無需獲得 root 存取權或干預列印機內部韌體。只需有權存取 Klipper 自訂巨集即可。

## 結果

```text
M141 S50 -> target = 50 -> iHeater Link 啟動加熱
M141 S0  -> target = 0  -> iHeater Link 關閉加熱
```

許多現代列印機已在室內內部有溫度感測器。如果製造商規定此類感測器並在 Klipper 組態中可見，則可用於將實際溫度傳輸到入口網站和 iHeater Link。如果沒有感測器，iHeater Link 仍可根據目標溫度管理加熱。

## 1. 新增巨集檔

在 Klipper 組態中建立 `virtual_chamber.cfg` 檔案，並從 `printer.cfg` 包含它：

```ini
[include virtual_chamber.cfg]
```

`virtual_chamber.cfg` 的內容：

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

儲存後，重新啟動 Klipper 或執行 `RESTART`。

## 2. 選擇性地連接室溫感測器

打開列印機組態並查看是否有類似於室溫感測器的物件。不同製造商和組建中可能有不同的命名，例如：

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

如果此類物件存在，可以將實際溫度傳輸到 iHeater Link。將以下區塊新增到 `virtual_chamber.cfg` 中，並將 `heater_generic chamber` 替換為您組態中物件的名稱：

```ini
[delayed_gcode UPDATE_VIRTUAL_CHAMBER_TEMP]
initial_duration: 1.0
gcode:
  {{ "{%" }} set t = printer["heater_generic chamber"].temperature|float {{ "%}" }}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=temperature VALUE={t}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=has_sensor VALUE=1
  UPDATE_DELAYED_GCODE ID=UPDATE_VIRTUAL_CHAMBER_TEMP DURATION=2.0
```

例如，如果您的感測器被描述為 `[temperature_sensor enclosure]`，溫度讀取線應參考 `printer["temperature_sensor enclosure"].temperature`。

如果沒有感測器或您不確定，請跳過此步驟。管理加熱足以用 `target`，由 `M141` 和 `M191` 巨集傳遞。

## 3. 在 iHeater Link 中啟用 Klipper 整合

在入口網站中打開 iHeater Link 裝置，並在 **Device Info** 區塊中按下 **MOONRAKER**。在介面中，此名稱用於 Klipper 列印機。

![在 Device Info 中選擇 Moonraker](../../img/iheater-link-settings-01.png)

然後按下裝置卡中的齒輪圖示、打開連接設定，並啟用 **MOONRAKER**。

![裝置設定](../../img/iheater-link-settings-03.png)

![在裝置設定中啟用 Moonraker](../../img/iheater-link-settings-04.png)

返回 **MOONRAKER** 設定、指定列印機的 IP 位址並儲存設定。

![返回 Moonraker 設定](../../img/iheater-link-settings-05.png)

![Moonraker 設定](../../img/iheater-link-settings-02.png)

通常這些參數就足夠了：

- Host：列印機在區域網路中的 IP 位址；
- Port：`7125`；
- API key：如果列印機不需要，請將其留空；
- Use SSL (wss)：對於普通本機連接，禁用；
- Poll interval：`1000`。

儲存後，iHeater Link 將開始讀取 Klipper 中的 `VIRTUAL_CHAMBER.target` 並將其傳遞給 iHeater。

## 4. 從入口網站手動管理

無需切片軟體即可啟動加熱：在裝置卡中設定室溫，然後按下 **START**。在時間欄中，可以以分鐘為單位指定加熱持續時間。如果將時間設定為 `0`，iHeater 將無限期地運行，直到您按下 **STOP** 或發送關閉命令。

![手動啟動 iHeater](../../img/iheater-link-settings-03.png)

## 5. 驗證巨集運作

在 Klipper 主控臺中執行：

```gcode
M141 S50
```

iHeater Link 應接收 `target=50` 並啟動 iHeater 加熱。

然後執行：

```gcode
M141 S0
```

Target 將變為 `0`，iHeater Link 將關閉加熱。

## 6. 設定切片軟體

切片軟體無需了解 `VIRTUAL_CHAMBER`。它應發送標準室溫命令：

- `M141 S{T}` — 在不等待的情況下設定室溫；
- `M191 S{T}` — 等待情況下設定室溫。

Klipper 巨集將攔截這些命令並將值寫入 `VIRTUAL_CHAMBER.target`。

### OrcaSlicer / Bambu Studio

室溫在絲材設定檔中設定：

```text
Filament Settings -> Temperatures -> Chamber temperature
```

例如：

- ABS / ASA：`40-50 °C`；
- PLA：`0 °C`。

檢查 G 碼的開頭。應該會出現類似以下的一行：

```gcode
M141 S45
```

### PrusaSlicer / SuperSlicer

如果設定檔具有室溫欄位，請使用它。如果沒有欄位，將命令手動新增到 Start G-code：

```gcode
M141 S45 ; chamber temperature for this filament
```

## 7. 務必在列印結束時關閉加熱

列印結束後，target 不會自動重置。將以下內容新增到 End G-code：

```gcode
CLEAR_VIRTUAL_CHAMBER
```

或：

```gcode
M141 S0
```

這將重置 `VIRTUAL_CHAMBER.target` 為 `0`，之後 iHeater Link 將關閉 iHeater。
