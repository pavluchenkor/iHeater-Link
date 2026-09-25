# Home Assistant

iHeater Link 透過 **MQTT Discovery** 在 Home Assistant 中發布自己：HA 會自行建立實體——溫度、加熱功率、啟動欄位與按鈕。這不需要入口，全部經由您的 MQTT 代理伺服器完成。

以下是啟用整合、檢查，以及現成的卡片版面配置，讓裝置看起來整齊，而不是一串實體清單。

![Home Assistant 中的 iHeater Link 卡片](../../img/ha-card.png)
*腔室溫度、加熱功率與啟動加熱集中在一個區塊。*

!!! note
    裝置**不會出現**在 `Settings → Devices & services → Discovered` 中：這是 MQTT Discovery，而不是 UPnP/zeroconf。Home Assistant 中的 **MQTT** 整合必須事先新增。

## 需要準備什麼

1. MQTT 代理伺服器：Home Assistant 中的 **Mosquitto broker** 附加元件，或您網路中的任何代理伺服器。
2. Home Assistant 中已新增指向該代理伺服器的 **MQTT** 整合。
3. iHeater Link 已連上網路，並在入口中顯示 `Online`。

!!! info "iHeater Link 是 iHeater 控制器的通訊模組；請為控制器燒錄 [iheater_revX_X_pulse](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases) 韌體。"

## 步驟 1. 在裝置上啟用整合

在 [portal.idryer.org](https://portal.idryer.org/) 上開啟裝置，找到 **整合** → **Home Assistant** 區塊。

| 欄位 | 填寫內容 |
|---|---|
| Host | 您網路中代理伺服器的位址，例如 `192.168.1.27` |
| Port | 代理伺服器的連接埠，通常是 `1883` |
| Username / Password | 代理伺服器的認證資訊（如果需要） |
| Discovery prefix | `homeassistant`，如果未在 HA 設定中更改過 |
| 啟用 | 勾選——否則裝置不會連接到代理伺服器 |

設定會透過本機網路直接送到裝置——入口不會保存這些設定。Home Assistant 由自己的開關啟用，不會干擾印表機整合：Bambu Lab 與 Moonraker 另外選擇，且同時只有其中一個運作。

![入口「整合」區塊中的 Home Assistant 視窗](../../img/ha-portal-integration.png)
*代理伺服器位址、連接埠和「啟用」標記——這就是裝置所需的全部內容。*

## 步驟 2. 在 Home Assistant 中找到裝置

在側邊選單底部點擊 **Settings**。

![側邊選單中的 Settings](../../img/HA-integration-01.png)

選擇 **Devices & services**。

![Devices & services](../../img/HA-integration-02.png)

找到 **MQTT** 卡片。名稱下方是已連接裝置的計數。

![整合清單中的 MQTT](../../img/HA-integration-03.png)

在 **Services** 區段中展開代理伺服器節點。iDryer 裝置以 `DEVICE_*` 形式的序號顯示。

![MQTT 裝置](../../img/HA-integration-04.png)

開啟裝置：HA 已經顯示讀數與控制元件。

![HA 中的裝置頁面](../../img/HA-integration-05.png)

## 步驟 3. 組建卡片

HA 會自行排列實體，結果是一長串清單。現成的版面配置把讀數放在上方，啟動加熱單獨成一個區塊。

1. `Settings` → `Dashboards` → **Add dashboard** → 空白儀表板，將其開啟。
2. 右上角 → 鉛筆（**Edit**）→「⋮」選單 → **Raw configuration editor**。
3. 貼上以下內容並儲存。

該版面配置適用於 `sections` 類型的儀表板。

```yaml
title: iDryer
views:
- title: Devices
  path: devices
  type: sections
  max_columns: 4
  sections:
  - type: grid
    background: true
    cards:
    - type: heading
      heading: iHeater Link
      heading_style: title
      icon: mdi:radiator
      badges:
      - type: entity
        entity: sensor.iheater_link_mode
        show_icon: false
        show_state: true
        color: primary
    - type: tile
      entity: sensor.iheater_link_temperature
      name: Temperature
      visibility:
      - condition: state
        entity: sensor.iheater_link_temperature
        state_not:
        - unknown
        - unavailable
    - type: tile
      entity: sensor.iheater_link_heater_power
      name: Heater power
    - type: heading
      heading: Heat
      heading_style: subtitle
    - type: tile
      entity: number.iheater_link_heat_temperature
      name: Temperature
      features:
      - type: numeric-input
        style: buttons
      features_position: bottom
    - type: tile
      entity: number.iheater_link_heat_duration
      name: Duration
      features:
      - type: numeric-input
        style: buttons
      features_position: bottom
    - type: tile
      entity: button.iheater_link_heat
      name: Start heating
      icon: mdi:play
      hide_state: true
      tap_action: &id001
        action: perform-action
        perform_action: button.press
        target:
          entity_id: button.iheater_link_heat
      icon_tap_action: *id001
    - type: tile
      entity: button.iheater_link_stop
      name: Stop
      icon: mdi:stop
      hide_state: true
      tap_action: &id002
        action: perform-action
        perform_action: button.press
        target:
          entity_id: button.iheater_link_stop
      icon_tap_action: *id002
```

![貼上版面配置後的 Raw configuration editor](../../img/ha-raw-editor.png)
*儀表板設定編輯器中的同一份版面配置。*

啟動順序與應用程式中相同：先設定溫度與持續時間，然後按下 **啟動加熱**。**停止** 按鈕關閉加熱。

## 如果實體名稱不一致

該版面配置基於 `sensor.iheater_link_temperature` 這類標準識別碼。如果卡片顯示「Entity not found」，請查看您自己的識別碼：`Settings` → `Devices & services` → **MQTT** → 您的裝置 → 實體清單，然後將版面配置中的前綴換成您自己的。

## 底層運作方式

- 實體的組成由裝置自行宣告——來自它自己的卡片描述：讀數、參數欄位與動作按鈕。裝置沒有的東西，不會出現在 Home Assistant 中。
- 數值與一般遙測一起發布到 MQTT；HA 即時收到它們。
- 在 HA 中按下按鈕，傳到裝置的是與來自入口或應用程式相同的動作——韌體中沒有專為 Home Assistant 設計的獨立邏輯。

## 診斷

| 症狀 | 檢查內容 |
|---|---|
| 裝置未出現在 HA 中 | 入口中 Home Assistant 整合已勾選「啟用」，代理伺服器的位址與連接埠正確。裝置必須為 `Online`。 |
| 出現了，但數值為 `Unknown` | 等待一個遙測週期。如果仍然為空——代理伺服器不保存 retained 訊息，或裝置未連接到它。 |
| 沒有腔室溫度 | 感測器未連接到 iHeater 控制器：沒有它，裝置不會發布這個數值。 |
| 按鈕沒有反應 | 檢查代理伺服器是否允許發布到 `idryer/#` 主題，以及裝置日誌中是否有授權錯誤。 |
| 數值為 `Unknown` 的幽靈實體 | 舊韌體遺留的 retained 訊息。清除方式：`mosquitto_pub -h <代理伺服器> -t 'homeassistant/<...>/config' -n -r`。 |
| 啟用 Home Assistant 後 Bambu 或 Moonraker 消失了 | 與 Home Assistant 無關——它是獨立啟用的。請檢查印表機整合的選擇：同時只有其中一個運作。 |
