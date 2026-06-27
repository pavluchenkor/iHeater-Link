# iHeater Link — 快速指南

iHeater Link 是用於 **iHeater** 控制器的通訊模組，搭配 [iheater_revХ_Х_pulse](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases) 韌體。這是一塊基於 ESP32-C3 / ESP32-S3 的板子，具有以下功能：

- 連接到 Wi-Fi，將 iHeater 與 [portal.idryer.org](https://portal.idryer.org/) 相連。
- 透過以下集成方式從印表機接收烘乾室目標溫度：
  **Moonraker (Klipper)**、**Bambu Lab (LAN)** 或
  **Home Assistant**。
- 將目標溫度轉換為脈衝信號，並透過單個 GPIO 傳輸至 iHeater 控制器。

iHeater 的控制是「有線」進行的：一條信號引腳 ESP → iHeater 信號輸入。Wi-Fi 和集成是 Link 的責任，加熱和安全是 iHeater 的責任。

單線連接對 Link 的放置位置沒有限制。ESP 板可以放在熱室外。這排除了：

- 當室溫在 60°C 以上運行時芯片和周邊裝置過熱；
- 長時間加熱時無線電部分熱停滯和 Wi-Fi 會話斷開；
- 加速退化。

只有 iHeater 留在室內，其設計用於在高溫下工作。信號線到 ESP 的長度僅受合理的線路負載限制（數十厘米——無須擔憂）。

## 支持的板子

| 板子                        |       |
| ---------------------------- | ----- |
| ESP32-C3 Super Mini          | `✅`  |
| ESP32-C3 DevKitM-1           | `✅`  |
| Seeed XIAO ESP32-S3          | `✅`  |
| Waveshare ESP32-S3-Zero      | `✅`  |

任何其他基於 ESP32-C3 或 ESP32-S3 的板子都可以使用，只要有可用的 GPIO 用於信號輸出。參考製造商的引腳配置。

## 接線圖

!!! warning "切勿在供電狀態下連接或斷開電線。"

電源透過 USB-C 供應到 ESP。ESP 反過來透過 5V 線為 iHeater 控制器供電。這是最簡單的方案。如有必要，iHeater 的電源可以以其他方式組織——與 Link 的通訊不依賴於電源方案。

![連接 ESP32-C3 Super Mini 到 iHeater](../img/iHeaterLink.png)

連接（適用於所有支持的板子）：

| ESP        | iHeater          | 用途              |
| ---------- | ---------------- | --------------------- |
| `5V`       | `5V`             | 控制器電源        |
| `GND`      | `GND`            | 公共接地          |
| `GPIO3`    | 信號輸入         | 脈衝 setpoint     |

### 板子引腳配置

ESP32-C3 Super Mini：

![ESP32-C3 Super Mini 引腳配置](../img/ESP32-C3-Super-Mini-pinout-low.jpg)

Waveshare ESP32-S3-Zero：

![Waveshare ESP32-S3-Zero 引腳配置](../img/ESP32-C3-ZERO-Waveshare-pinout-low.jpg)

## 透過網頁 Flasher 進行韌體刷寫

網頁 Flasher 位於 [install.idryer.org](https://install.idryer.org/)。

1. 將 Link 連接到計算機的 USB 連接埠。
2. 打開 [install.idryer.org](https://install.idryer.org/)，選擇 **iHeater Link** 設備。
3. 選擇板子變體。
4. 按下 **Connect**，選擇串行連接埠（通常是 `USB JTAG/serial` 或 `CH340`）。如果設備未被識別，按住板上的 `BOOT` 按鈕並短按一次 `RST`。
5. 按下 **Install**。Flasher 將寫入韌體。
6. 韌體刷寫完成後，Wi-Fi 設置嚮導將打開。

## Wi-Fi 設置

刷寫後，Improv 嚮導會自動在串行連接埠中打開。

1. 輸入您的 2.4 GHz 網絡的 SSID 和密碼。
2. 等待狀態變為 **Connected**。Link 的指示燈將進入「呼吸」藍光模式。

如果嚮導未打開，請斷開 USB，然後透過 **Connect** 重新連接而無需重新刷寫。

!!! note "ESP32 僅支持 2.4 GHz。5 GHz 網絡不兼容。"

## 綁定到入口網站

1. 在 Flasher 頁面上，按下 **連接並執行聲明**。
2. 設備將收到 `START_CLAIM` 命令。幾秒鐘後，頁面上將出現 PIN。PIN 有效期約為 5 分鐘。
3. 打開 [portal.idryer.org](https://portal.idryer.org/) → **添加設備** → 輸入 PIN。
4. 成功綁定後，設備將顯示在線列表中。

如果收到 `CLAIM_ALREADY:DEVICE_…` 響應——設備已綁定到此帳戶或其他帳戶。在這種情況下，在入口網站中刪除設備並重複綁定。

## 連接到 iHeater

1. 關閉控制器電源。
2. 根據上方的圖表將 ESP 連接到 iHeater：`5V`、`GND`、`GPIO3` → iHeater 信號輸入。
3. 為 ESP USB 供電。控制器將透過 5V 線供電。

加載後，Link 將建立與入口網站的連接，激活選定的集成，並開始將烘乾室目標溫度傳輸至 iHeater。

## 預期效果

- LED 1 持續點亮，LED 3 每秒短閃 1 次，這表示 iHeater Link - iHeater 有連接。
- 連接丟失時，所有 LED 以 1 Hz 頻率閃爍。
- 其他錯誤與 iHeater Link 相關，重複獨立韌體的指示。

## 診斷

在設備菜單中有 **DIAGNOSTICS → DIAG LOG** 選項。啟用時，Serial 每秒輸出詳細報告：Wi-Fi 狀態、MQTT、活躍集成、當前目標、連接器錯誤。

有關診斷的更多詳情，請參閱 idryer-core 庫文檔，可在 GitHub 上的項目存儲庫中獲得。
