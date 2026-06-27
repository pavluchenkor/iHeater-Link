# 將 iHeater Link 連接至 Home Assistant

iHeater Link 通過 MQTT Discovery 在 Home Assistant 中發布設備：HA 會自動創建帶有真實傳感器和控制元素的卡片（目標溫度、持續時間、IDLE/DRYING/STORAGE 模式）。

!!! note
    設備**不會出現**在 `Settings → Devices & services → Discovered` 中。iHeater Link 使用 MQTT Discovery，而不是 UPnP/zeroconf。Home Assistant 必須**已經添加**了 **MQTT** 集成，並指向您的代理。

## 應該準備好什麼

1. MQTT 代理（例如 Mosquitto 附加組件）在 HA 中運行或可通過網絡訪問。
2. 在 HA 中添加了帶有已配置代理的 **MQTT** 集成。
3. iHeater Link 通過門戶收到 `link_integration {type:"ha"}` 命令並與相同的代理建立了連接。

## 步驟 1. 打開設置

在 Home Assistant 側邊菜單底部點擊 **Settings**。

![側邊菜單中的 Settings](../../img/HA-integration-01.png)

## 步驟 2. 進入 Devices & services

在設置部分列表中選擇 **Devices & services**。

![Devices & services](../../img/HA-integration-02.png)

## 步驟 3. 打開 MQTT 集成

在集成列表中找到 **MQTT** 卡片。名稱下方是已連接設備的計數器。

![集成列表中的 MQTT](../../img/HA-integration-03.png)

## 步驟 4. 找到 iDryer 設備

在集成頁面的 **Services** 部分中，展開代理節點（`127.0.0.1` 或您的代理地址）。其下列出了 iDryer 設備及其 `DEVICE_*` 形式的序列號。

![MQTT 設備](../../img/HA-integration-04.png)

點擊所需的設備。

## 步驟 5. 控制和狀態

在設備頁面上有兩個塊：

- **Controls** — 控制元素：
  - `iDryer U1 duration` — 持續時間（分鐘）
  - `iDryer U1 mode control` — 模式（`IDLE` / `DRYING` / `STORAGE`）
  - `iDryer U1 target temp` — 目標溫度（滑塊）
- **Sensors** — 實際值。構成取決於設備類型（`Config` 決定發布的傳感器）：
  - iHeater Link：`heater_power`、`mode`、`alerts`
  - Storage Link：相同加上 `temperature`、`humidity`

![HA 中的設備頁面](../../img/HA-integration-05.png)

要開始加熱：

1. 用滑塊設置目標溫度。
2. 設置持續時間。
3. 在選擇器中選擇 `DRYING` 或 `STORAGE` 模式。

要停止 — 將選擇器切換為 `IDLE`。

!!! note
    `target temp` 和 `duration` 值首先作為「待處理」保存在設備上，實際啟動發生在選擇模式時。這允許以任何順序設置參數並通過一個操作啟動。

## 底層發生了什麼

- **Discovery**（在 HA UI 中創建具有正確圖標的實體）— 連接到 HA 代理時自動發布。構成由 `Config.hasXxx` 標誌決定 — 不存在的傳感器不會以幽靈形式出現。
- **State**（當前值）— 每 5 秒發布到 HA 主題，同時發布到門戶。
- **Commands**（`set_temp` / `set_duration` / `set_mode`）— 來自 HA → MQTT 代理 → 設備 → 組合成 `Request` 並遵循與門戶命令相同的路徑。產品代碼中沒有 HA 特定的分支。

## 診斷

| 症狀 | 要檢查的內容 |
|---------|---------------|
| 設備未在 HA 中出現 | 在門戶中的設備上 — `Home Assistant → Enabled: yes`。`integrations/status` 中的 `ha.state` 字段應為 `online`。|
| Discovery 已發布，但卡片為空 | 等待 5–10 秒後進行首次連接。如果值未出現 — 檢查 MQTT 代理是否未丟失 retained 消息。|
| 控制按鈕無響應 | 檢查 Discovery 中的 `command_topic` — 主題應與 `idryer/{serial}/U1/set_mode` 等一致。|
| 帶有 Unknown 值的幽靈傳感器 | 來自舊固件版本的舊 retained Discovery。更新後，要麼等待下一個 Discovery 發布週期，要麼清除 retained：`mosquitto_pub -t 'homeassistant/.../config' -n -r`。|
