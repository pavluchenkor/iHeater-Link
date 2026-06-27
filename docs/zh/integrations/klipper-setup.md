# 为 iHeater Link 设置 Klipper

## 为什么需要这个功能

此功能适用于搭载 Klipper 的闭源 3D 打印机：Creality、Qidi、Flashforge 及其他现代型号，在这些机器上用户无法自行编译和安装 iHeater 固件以直接与 Klipper 通信。

通常情况下，Klipper 配置文件和用户自定义 G-code 宏仍然是可访问的。因此 iHeater Link 采用更简单的方式：它连接到与打印机相同的 Wi-Fi 网络，读取用户 Klipper 宏中的变量，并将目标温度传送到 iHeater 控制器。

在打印机端只需添加几个 G-code 宏。这些宏接收标准的腔室温度命令 `M141` 和 `M191`，并将目标温度保存到 `VIRTUAL_CHAMBER`。

最终方案如下：

```text
切片软件 / G-code -> M141 S50 -> Klipper macro VIRTUAL_CHAMBER.target=50
                                      |
                                      v
本地网络中的打印机 <- Wi-Fi <- iHeater Link -> 信号线 -> iHeater
```

用户不需要获取 root 权限或修改打印机内部固件。只需能够访问 Klipper 用户宏即可。

## 实现效果

```text
M141 S50 -> target = 50 -> iHeater Link 启动加热
M141 S0  -> target = 0  -> iHeater Link 停止加热
```

许多现代打印机的腔室内已装有温度传感器。如果制造商配备了此传感器并且在 Klipper 配置中可见，可用其将实际温度传送到门户网站和 iHeater Link。如果没有传感器，iHeater Link 仍可根据目标温度进行加热控制。

## 1. 添加宏文件

在 Klipper 配置中创建文件 `virtual_chamber.cfg`，并从 `printer.cfg` 中引入：

```ini
[include virtual_chamber.cfg]
```

`virtual_chamber.cfg` 的内容：

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

保存后重启 Klipper 或执行 `RESTART`。

## 2. 可选：连接腔室传感器

打开打印机配置文件，查看是否存在类似腔室温度传感器的对象。不同制造商和构建方案中，它可能有不同的名称，例如：

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

如果存在这样的对象，可以将实际温度传送给 iHeater Link。在 `virtual_chamber.cfg` 中添加下面的块，并将 `heater_generic chamber` 替换为您配置中的对象名称：

```ini
[delayed_gcode UPDATE_VIRTUAL_CHAMBER_TEMP]
initial_duration: 1.0
gcode:
  {{ "{%" }} set t = printer["heater_generic chamber"].temperature|float {{ "%}" }}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=temperature VALUE={t}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=has_sensor VALUE=1
  UPDATE_DELAYED_GCODE ID=UPDATE_VIRTUAL_CHAMBER_TEMP DURATION=2.0
```

例如，如果您的传感器定义为 `[temperature_sensor enclosure]`，温度读取行应引用 `printer["temperature_sensor enclosure"].temperature`。

如果没有传感器或您不确定，可以跳过此步骤。控制加热只需要 `M141` 和 `M191` 宏传送的 `target`。

## 3. 在 iHeater Link 中启用 Klipper 集成

在门户网站中打开 iHeater Link 设备，在**设备信息**块中点击 **MOONRAKER**。在此界面中，此名称用于 Klipper 打印机。

![在设备信息中选择 Moonraker](../../img/iheater-link-settings-01.png)

然后点击设备卡上的齿轮图标，打开连接设置并启用 **MOONRAKER**。

![设备设置](../../img/iheater-link-settings-03.png)

![在设备设置中启用 Moonraker](../../img/iheater-link-settings-04.png)

返回 **MOONRAKER** 设置，输入打印机的 IP 地址并保存设置。

![返回 Moonraker 设置](../../img/iheater-link-settings-05.png)

![Moonraker 设置](../../img/iheater-link-settings-02.png)

通常这些参数就足够了：

- Host：打印机在本地网络中的 IP 地址；
- Port：`7125`；
- API key：如果打印机不需要，留空即可；
- Use SSL (wss)：对于普通本地连接，保持关闭；
- Poll interval：`1000`。

保存后，iHeater Link 将开始从 Klipper 读取 `VIRTUAL_CHAMBER.target` 并将其传送到 iHeater。

## 4. 从门户网站手动控制

即使不通过切片软件也可以启动加热：在设备卡中设置腔室温度，然后点击 **START**。在时间字段中可以指定加热持续时间（分钟）。如果将时间设为 `0`，iHeater 将持续工作，直到您点击 **STOP** 或发送关闭命令。

![手动启动 iHeater](../../img/iheater-link-settings-03.png)

## 5. 验证宏的工作

在 Klipper 控制台中执行：

```gcode
M141 S50
```

iHeater Link 应该接收到 `target=50` 并启动 iHeater 加热。

然后执行：

```gcode
M141 S0
```

Target 变为 `0`，iHeater Link 将关闭加热。

## 6. 配置切片软件

切片软件不需要了解 `VIRTUAL_CHAMBER`。它应该发送标准的腔室温度命令：

- `M141 S{T}` — 设置腔室温度，不等待；
- `M191 S{T}` — 设置腔室温度，等待达到。

Klipper 宏将拦截这些命令并将值写入 `VIRTUAL_CHAMBER.target`。

### OrcaSlicer / Bambu Studio

腔室温度在耗材配置文件中设置：

```text
Filament Settings -> Temperatures -> Chamber temperature
```

例如：

- ABS / ASA：`40-50 °C`；
- PLA：`0 °C`。

检查 G-code 开始部分。应该会出现类似以下的一行：

```gcode
M141 S45
```

### PrusaSlicer / SuperSlicer

如果配置文件中有腔室温度字段，使用它。如果没有此字段，在 Start G-code 中手动添加命令：

```gcode
M141 S45 ; chamber temperature for this filament
```

## 7. 打印结束时必须关闭加热

打印完成后，target 不会自动重置。在 End G-code 中添加：

```gcode
CLEAR_VIRTUAL_CHAMBER
```

或者：

```gcode
M141 S0
```

这将把 `VIRTUAL_CHAMBER.target` 重置为 `0`，之后 iHeater Link 将关闭 iHeater。
