# Bambu Lab 的 iHeater Link 设置

## 为什么需要这个

iHeater Link 可以在 Bambu Lab 打印机上打印时自动控制 iHeater。与 Klipper 方案不同，无需在打印机上添加宏或修改 G 代码。Link 通过局域网连接到打印机并读取打印状态和活跃耗材。

在当前的 Bambu Lab 打印机上，无法可靠地使用腔室温度传输作为 iHeater 控制命令。但是 Link 可以确定活跃料架中当前安装的耗材类型，以及打印机是否已开始预热或正在打印。根据耗材类型，固件从材料表中选择所需的腔室温度，并自动打开 iHeater。

如果固件材料列表中没有所需的耗材类型，请联系固件作者：该类型可在后续版本中添加。

## 最终效果

```text
Bambu 开始预热或打印 -> Link 检测到活跃耗材 -> 从材料表中选择温度 -> iHeater 打开
```

当打印结束或活跃工艺不再需要加热时，Link 会关闭 iHeater。

## 1. 打开设备设置

在门户网站上打开 iHeater Link 卡片，然后点击齿轮图标。

![打开设备设置](../../img/iheater-link-settings-bambu-02.png)

## 2. 启用 Bambu 连接

在设备设置中打开 **CONNECTIONS** 块并启用 **BAMBU**。如果不使用其他连接，可以将其保持禁用状态。

![在设备设置中启用 Bambu](../../img/iheater-link-settings-bambu-03.png)

## 3. 在设备页面选择 Bambu Lab

返回设备页面，然后在 **Device Info** 块中点击 **BAMBU LAB**。按钮将变为活跃状态。

![选择 Bambu Lab](../../img/iheater-link-settings-bambu-04.png)

## 4. 输入连接参数

在 **Bambu Lab** 设置中启用集成并填写连接参数。

![Bambu Lab 设置](../../img/iheater-link-settings-bambu-05.png)

通常需要以下字段：

- Printer IP：打印机在局域网中的 IP 地址；
- Printer serial：打印机序列号；
- LAN access code：LAN 模式访问码；
- Auto-apply on tag detect：如果 Link 应根据检测到的耗材自动应用温度，请启用；
- Default AMS 和 Default tray：如果不需要强制选择特定的 AMS 或料架，可以保留默认值。

打印机和 iHeater Link 必须在同一局域网中。LAN access code 和序列号可从 Bambu Lab 打印机设置中获得。

## 5. 配置材料温度

在设备设置中打开 **MATERIALS** 块。可以为每种耗材类型设置各自的腔室温度。

![材料温度](../../img/iheater-link-settings-001-materials.png)

固件已包含广泛的耗材类型。当打印机开始预热或打印时，Link 将检查活跃料架，确定耗材类型并从该表中获取温度。例如，PLA 可以设置较低的温度或禁用加热，而 ABS 和 ASA 则设置更高的温度。

如果在表中找不到活跃耗材或为其设置的温度不合适，请在 **MATERIALS** 中编辑该值并保存设置。

## 6. 验证工作

在 Bambu Lab 上使用已在 **MATERIALS** 中设置温度的耗材启动打印。当打印机进入预热或打印时，iHeater Link 应自动为活跃耗材应用温度并打开 iHeater。

如果加热不打开，请检查：

- 设备设置中是否启用了 **BAMBU** 连接；
- 是否在 **Device Info** 块中选择了 **BAMBU LAB**；
- 是否正确输入了 IP、serial 和 LAN access code；
- 打印机是否识别了活跃料架和耗材类型；
- 是否在 **MATERIALS** 中为该耗材类型设置了温度。
