# Home Assistant

iHeater Link 通过 **MQTT Discovery** 把自己发布到 Home Assistant：HA 自行创建实体 —— 温度、加热功率、启动参数字段和按钮。这不需要门户，全部通过您的 MQTT 代理完成。

下面是启用集成、检查以及现成的卡片布局，让设备显示得整齐，而不是一串实体列表。

![Home Assistant 中的 iHeater Link 卡片](../../img/iheater-ha-card.png)
*腔室温度、加热功率和启动加热在同一个区块中。*

!!! note
    设备**不会出现**在 `Settings → Devices & services → Discovered` 中：这是 MQTT Discovery，不是 UPnP/zeroconf。Home Assistant 中必须事先添加 **MQTT** 集成。

## 需要什么

1. MQTT 代理：Home Assistant 中的 **Mosquitto broker** 加载项，或您网络中的任意代理。
2. Home Assistant 中已添加指向该代理的 **MQTT** 集成。
3. iHeater Link 已接入网络，并在门户上显示 `Online`。

!!! info "iHeater Link —— iHeater 控制器的通信模块；请为控制器刷写 [iheater_revX_X_pulse](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases) 固件。"

## 步骤 1. 在设备上启用集成

在 [portal.idryer.org](https://portal.idryer.org/) 上打开设备，找到 **集成** → **Home Assistant** 区块。

| 字段 | 填写内容 |
|---|---|
| Host | 您网络中代理的地址，例如 `192.168.1.27` |
| Port | 代理端口，通常是 `1883` |
| Username / Password | 代理的凭据，如果代理要求的话 |
| Discovery prefix | `homeassistant`，如果没有在 HA 设置中改过 |
| 已启用 | 勾选 —— 否则设备不会连接到代理 |

设置通过局域网直接发送到设备 —— 门户不保存它们。Home Assistant 用自己的开关启用，不会影响打印机集成：Bambu Lab 和 Moonraker 单独选择，同时只有其中一个工作。

![门户「集成」区块中的 Home Assistant 窗口](../../img/iheater-ha-portal-integration.png)
*代理地址、端口和「已启用」标记 —— 设备需要的全部内容。*

## 步骤 2. 在 Home Assistant 中找到设备

在侧边栏底部点击 **Settings**。

![侧边栏中的 Settings](../../img/HA-integration-01.png)

选择 **Devices & services**。

![Devices & services](../../img/HA-integration-02.png)

找到 **MQTT** 卡片。名称下方是已连接设备的计数。

![集成列表中的 MQTT](../../img/HA-integration-03.png)

在 **Services** 部分展开代理节点。iDryer 设备以 `DEVICE_*` 形式的序列号显示。

![MQTT 设备](../../img/HA-integration-04.png)

打开设备：HA 已经显示读数和控制元素。

![HA 中的设备页面](../../img/HA-integration-05.png)

## 步骤 3. 组装卡片

HA 自行排列实体，结果是一长串列表。现成的布局把读数放在上方，把启动加热放成单独的区块。

1. `Settings` → `Dashboards` → **Add dashboard** → 空仪表板，打开它。
2. 右上角 → 铅笔（**Edit**）→「⋮」菜单 → **Raw configuration editor**。
3. 粘贴下面的内容并保存。

该布局适用于 `sections` 类型的仪表板。

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

![粘贴了布局的 Raw configuration editor](../../img/iheater-ha-raw-editor.png)
*仪表板配置编辑器中的同一布局。*

启动顺序和应用中一样：先设定温度和持续时间，然后按 **启动加热**。**停止** 按钮关闭加热。

## 如果实体名称对不上

该布局针对 `sensor.iheater_link_temperature` 这类标准标识符。如果卡片显示「Entity not found」，请查看自己的名称：`Settings` → `Devices & services` → **MQTT** → 您的设备 → 实体列表，然后把布局中的前缀换成您自己的。

## 幕后发生了什么

- 实体的组成由设备自己声明 —— 来自它自身卡片的描述：读数、参数字段和动作按钮。设备没有的东西，不会出现在 Home Assistant 中。
- 数值和普通遥测一起发布到 MQTT；HA 实时接收它们。
- 在 HA 中按下按钮，到达设备时与从门户或应用发出的动作完全相同 —— 固件中没有「专为 Home Assistant」的单独逻辑。

## 诊断

| 症状 | 检查内容 |
|---|---|
| 设备没有出现在 HA 中 | 门户上 Home Assistant 集成勾选了「已启用」，代理地址和端口正确。设备必须是 `Online`。 |
| 出现了，但数值是 `Unknown` | 等待一个遥测周期。如果之后仍然为空 —— 代理不保存 retained 消息，或者设备没有连上它。 |
| 没有腔室温度 | 传感器没有接到 iHeater 控制器：没有传感器时设备不会发布这个数值。 |
| 按钮没有反应 | 检查代理是否允许发布到 `idryer/#` 主题，以及设备日志中有没有授权错误。 |
| 数值为 `Unknown` 的幽灵实体 | 旧固件残留的 retained 消息。清除：`mosquitto_pub -h <代理> -t 'homeassistant/<...>/config' -n -r`。 |
| Home Assistant 一起用之后 Bambu 或 Moonraker 消失了 | 这与 Home Assistant 无关 —— 它是单独启用的。检查打印机集成的选择：同时只有其中一个工作。 |
