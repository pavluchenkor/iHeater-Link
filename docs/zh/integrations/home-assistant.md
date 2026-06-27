# 将 iHeater Link 连接到 Home Assistant

iHeater Link 通过 MQTT Discovery 将设备发布到 Home Assistant：HA 自动创建卡片，包含真实的传感器和控制元素（目标温度、持续时间、IDLE/DRYING/STORAGE 模式）。

!!! note
    设备**不会出现**在 `Settings → Devices & services → Discovered` 中。iHeater Link 使用 MQTT Discovery，而不是 UPnP/zeroconf。Home Assistant 必须**已添加** **MQTT** 集成，指向您的代理。

## 需要准备什么

1. MQTT 代理（例如 Mosquitto add-on）在 HA 中运行或通过网络可访问。
2. 在 HA 中添加了配置好的代理的 **MQTT** 集成。
3. iHeater Link 通过门户网站收到命令 `link_integration {type:"ha"}` 并与同一代理建立了连接。

## 步骤 1. 打开设置

在 Home Assistant 侧边栏底部，点击 **Settings**。

![侧边栏中的 Settings](../../img/HA-integration-01.png)

## 步骤 2. 转到 Devices & services

在设置部分列表中，选择 **Devices & services**。

![Devices & services](../../img/HA-integration-02.png)

## 步骤 3. 打开 MQTT 集成

在集成列表中找到 **MQTT** 卡片。在名称下方是已连接设备的计数器。

![集成列表中的 MQTT](../../img/HA-integration-03.png)

## 步骤 4. 查找 iDryer 设备

在集成页面的 **Services** 部分，展开代理节点（`127.0.0.1` 或您的代理地址）。下面列出了 iDryer 设备及其序列号，格式为 `DEVICE_*`。

![MQTT 设备](../../img/HA-integration-04.png)

点击所需的设备。

## 步骤 5. 管理和状态

设备页面有两个块：

- **Controls** — 控制元素：
  - `iDryer U1 duration` — 持续时间（分钟）
  - `iDryer U1 mode control` — 模式（`IDLE` / `DRYING` / `STORAGE`）
  - `iDryer U1 target temp` — 目标温度（滑块）
- **Sensors** — 真实值。组成取决于设备类型（`Config` 定义已发布的传感器）：
  - iHeater Link: `heater_power`, `mode`, `alerts`
  - Storage Link: 相同加 `temperature`, `humidity`

![HA 中的设备页面](../../img/HA-integration-05.png)

要启动加热：

1. 用滑块设置目标温度。
2. 设置持续时间。
3. 在选择器中选择 `DRYING` 或 `STORAGE` 模式。

要停止 — 将选择器切换到 `IDLE`。

!!! note
    值 `target temp` 和 `duration` 首先被保存到设备上作为"待处理"，实际启动发生在选择模式时。这允许以任意顺序设置参数并通过单个操作启动。

## 幕后发生了什么

- **Discovery**（在 HA UI 中创建具有正确图标的 entity）— 连接到 HA 代理时自动发布。组成由 `Config.hasXxx` 标志确定 — 不存在的传感器不会显示为幻影。
- **State**（当前值）— 每 5 秒发布到 HA 主题，并行发布到门户。
- **Commands**（`set_temp` / `set_duration` / `set_mode`）— 从 HA → MQTT 代理 → 设备，在 `Request` 中收集，并通过与门户命令相同的路径。产品代码中没有 HA 特定的分支。

## 诊断

| 症状 | 检查内容 |
|------|---------|
| 设备未出现在 HA 中 | 在门户中的设备上 — `Home Assistant → Включено: да`。`integrations/status` 中的 `ha.state` 字段应为 `online`。 |
| Discovery 已发布但卡片为空 | 在第一次连接后等待 5–10 秒。如果值仍未出现 — 检查 MQTT 代理是否没有丢失 retained 消息。 |
| 控制按钮无响应 | 检查 Discovery 中的 `command_topic` — 主题应与 `idryer/{serial}/U1/set_mode` 等匹配。 |
| 带有 Unknown 值的幻影传感器 | 来自旧固件版本的旧 retained Discovery。更新后，要么等待下一个 Discovery 发布周期，要么清除 retained：`mosquitto_pub -t 'homeassistant/.../config' -n -r`。 |
