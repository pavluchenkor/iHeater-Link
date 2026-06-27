# iHeater Link — 快速指南

iHeater Link — 用于 **iHeater** 控制器的通信模块，需要配合 [iheater_revХ_Х_pulse](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases) 固件。这是一块基于 ESP32-C3 / ESP32-S3 的主板，具有以下功能：

- 连接到 Wi-Fi 并将 iHeater 与 [portal.idryer.org](https://portal.idryer.org/) 关联。
- 通过集成 **Moonraker (Klipper)**、**Bambu Lab (LAN)** 或
  **Home Assistant** 从打印机获取腔室目标温度。
- 将目标温度转换为脉冲信号，并通过单个 GPIO 将其传输到 iHeater 控制器。

iHeater 的控制是"有线的"：一根信号引脚从 ESP → iHeater 信号输入。Wi-Fi 和集成由 Link 负责，加热和安全由 iHeater 负责。

单线连接对 Link 的放置位置没有限制。ESP 主板可以放置在热室之外。这可以避免：

- 腔室在 60°C 以上工作时芯片和外围设备过热；
- 长期加热时无线部分的热依赖振荡和 Wi-Fi 会话中断；
- 加速衰减。

热室内只保留设计用于高温工作的 iHeater。信号线到 ESP 的长度仅受合理的线路负载限制（几十厘米没有问题）。

## 支持的主板

| 主板                         |       |
| ---------------------------- | ----- |
| ESP32-C3 Super Mini          | `✅`  |
| ESP32-C3 DevKitM-1           | `✅`  |
| Seeed XIAO ESP32-S3          | `✅`  |
| Waveshare ESP32-S3-Zero      | `✅`  |

任何其他基于 ESP32-C3 或 ESP32-S3 的主板都可以使用，只要有空闲的 GPIO 用于信号输出。请查阅制造商的引脚定义。

## 接线图

!!! warning "切勿在通电时连接或断开导线。"

电源通过 USB-C 供应给 ESP。ESP 反过来通过 5 V 线为 iHeater 控制器供电。这是最简单的选择。如需要，可以以其他方式组织 iHeater 的电源 — Link 的通信不依赖于电源方案。

![将 ESP32-C3 Super Mini 连接到 iHeater](../img/iHeaterLink.png)

连接（对所有支持的主板）：

| ESP        | iHeater          | 用途              |
| ---------- | ---------------- | --------------------- |
| `5V`       | `5V`             | 控制器电源   |
| `GND`      | `GND`            | 公共接地           |
| `GPIO3`    | 信号输入  | 脉冲设定值   |

### 主板引脚定义

ESP32-C3 Super Mini：

![ESP32-C3 Super Mini 引脚定义](../img/ESP32-C3-Super-Mini-pinout-low.jpg)

Waveshare ESP32-S3-Zero：

![Waveshare ESP32-S3-Zero 引脚定义](../img/ESP32-C3-ZERO-Waveshare-pinout-low.jpg)

## 通过 Web 刷机工具刷固件

Web 刷机工具位于 [install.idryer.org](https://install.idryer.org/)。

1. 将 Link 连接到计算机的 USB 端口。
2. 打开 [install.idryer.org](https://install.idryer.org/) 并选择设备 **iHeater Link**。
3. 选择主板类型。
4. 单击**连接**，选择串行端口（通常是 `USB JTAG/serial` 或 `CH340`）。如果未识别设备，按住主板上的 `BOOT` 按钮并短按一次 `RST`。
5. 单击**安装**。刷机工具将写入固件。
6. 刷机完成后，Wi-Fi 设置向导将自动打开。

## 配置 Wi-Fi

刷机后，Improv 向导自动在串行端口打开。

1. 输入您的 2.4 GHz 网络的 SSID 和密码。
2. 等待状态显示**已连接**。Link 的指示灯将转入青蓝色"呼吸"模式。

如果向导未打开，请断开 USB 连接，然后通过**连接**重新连接，无需重新刷固件。

!!! note "ESP32 仅支持 2.4 GHz。5 GHz 网络无法工作。"

## 绑定到门户

1. 在刷机工具页面上单击**连接并执行 Claim**。
2. `START_CLAIM` 命令将发送到设备。几秒钟后，页面上将出现 PIN 码。PIN 码有效期约为 5 分钟。
3. 打开 [portal.idryer.org](https://portal.idryer.org/) → **添加设备** → 输入 PIN 码。
4. 成功绑定后，设备将出现在在线设备列表中。

如果收到响应 `CLAIM_ALREADY:DEVICE_…` — 设备已绑定到该帐户或另一个帐户。在这种情况下，在门户中删除设备并重新进行绑定。

## 连接到 iHeater

1. 关闭控制器电源。
2. 按照上述图表将 ESP 连接到 iHeater：`5V`、`GND`、`GPIO3` → iHeater 信号输入。
3. 为 ESP USB 供电。控制器将通过 5 V 线供电。

Link 加载后将建立与门户的连接，激活所选的集成，并开始将腔室目标温度传输到 iHeater。

## 应该看到什么

- LED 1 持续亮起，LED 3 每秒短闪 1 次，表示 iHeater Link - iHeater 连接正常。
- 连接丢失时，所有 LED 以 1 Hz 频率闪烁。
- 其他错误与 iHeater Link 相关，并重复独立固件的指示。

## 诊断

在设备菜单中有一个 **DIAGNOSTICS → DIAG LOG** 项目。启用后，串行端口每秒会输出详细报告：Wi-Fi 状态、MQTT、活动集成、当前目标值、连接器错误。

有关诊断的详细信息，请参见项目 GitHub 存储库中的 idryer-core 库文档。
