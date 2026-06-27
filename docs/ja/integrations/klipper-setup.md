# Klipper用iHeater Linkの設定

## 目的

この機能は、ファームウェアを自分でコンパイルしてインストールできないクローズドシステムのプリンタを対象としています：Creality、Qidi、Flashforgeなど、多くの最新モデルがこれに該当します。

通常、Klipperの設定ファイルとカスタムGコードマクロはアクセス可能です。したがって、iHeater Linkはより簡単な方法を使用します。プリンタと同じWi-Fiネットワークに接続し、Klipperのカスタムマクロ変数を読み取り、目標温度をiHeaterコントローラに送信します。

プリンタ側では、いくつかのGコードマクロを追加するだけです。これらのマクロは標準のチャンバー温度コマンド `M141` と `M191` を受け取り、目標温度を `VIRTUAL_CHAMBER` に保存します。

全体的なスキーム：

```text
スライサー / Gコード -> M141 S50 -> Klipperマクロ VIRTUAL_CHAMBER.target=50
                                      |
                                      v
ローカルネットワーク内のプリンタ <- Wi-Fi <- iHeater Link -> シグナルライン -> iHeater
```

ユーザがroot権限を取得したりプリンタの内部ファームウェアに干渉する必要はありません。Klipperのカスタムマクロへのアクセスがあれば十分です。

## 結果

```text
M141 S50 -> target = 50 -> iHeater Link がヒーティングを有効にします
M141 S0  -> target = 0  -> iHeater Link がヒーティングを無効にします
```

多くの最新プリンタには、すでにチャンバー内の温度センサがあります。このセンサがメーカーによって提供されており、Klipperの設定に表示されている場合、実際の温度をポータルとiHeater Linkに送信するために使用できます。センサがない場合、iHeater Linkはまだ目標温度でヒーティングを管理できます。

## 1. マクロファイルを追加する

Klipperの設定に `virtual_chamber.cfg` ファイルを作成し、`printer.cfg` から取り込みます：

```ini
[include virtual_chamber.cfg]
```

`virtual_chamber.cfg` の内容：

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

保存後、Klipperを再起動するか、`RESTART` を実行してください。

## 2. オプションでチャンバーセンサを接続する

プリンタの設定を開き、チャンバー温度センサに似たオブジェクトがあるかどうかを確認してください。異なるメーカーやビルドによって、異なる名前で呼ばれることがあります。例えば：

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

このようなオブジェクトがある場合、実際の温度をiHeater Linkに送信できます。下のブロックを `virtual_chamber.cfg` に追加し、`heater_generic chamber` を自分の設定からのオブジェクト名に置き換えてください：

```ini
[delayed_gcode UPDATE_VIRTUAL_CHAMBER_TEMP]
initial_duration: 1.0
gcode:
  {{ "{%" }} set t = printer["heater_generic chamber"].temperature|float {{ "%}" }}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=temperature VALUE={t}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=has_sensor VALUE=1
  UPDATE_DELAYED_GCODE ID=UPDATE_VIRTUAL_CHAMBER_TEMP DURATION=2.0
```

例えば、センサが `[temperature_sensor enclosure]` として記述されている場合、温度読み込み行は `printer["temperature_sensor enclosure"].temperature` を参照する必要があります。

センサがない場合、または確実でない場合は、このステップをスキップしてください。ヒーティング管理には、`M141` と `M191` のマクロが送信する `target` で十分です。

## 3. iHeater LinkでKlipper統合を有効にする

ポータルでiHeater Linkデバイスを開き、**デバイス情報**ブロックの **MOONRAKER** をクリックしてください。このインターフェースでは、Klipperプリンタにこの名前が使用されます。

![デバイス情報でMoonrakerを選択](../../img/iheater-link-settings-01.png)

次に、デバイスカード内のギアアイコンをクリックして、接続設定を開き、**MOONRAKER** を有効にしてください。

![デバイス設定](../../img/iheater-link-settings-03.png)

![デバイス設定でMoonrakerを有効にする](../../img/iheater-link-settings-04.png)

**MOONRAKER** 設定に戻り、プリンタのIPアドレスを指定して、設定を保存してください。

![Moonraker設定に戻る](../../img/iheater-link-settings-05.png)

![Moonraker設定](../../img/iheater-link-settings-02.png)

通常、以下のパラメータで十分です：

- Host：ローカルネットワーク内のプリンタのIPアドレス
- Port：`7125`
- API key：プリンタが必要としない場合は空のままにしてください
- Use SSL (wss)：通常のローカル接続では無効です
- Poll interval：`1000`

保存後、iHeater Linkは Klipperから `VIRTUAL_CHAMBER.target` を読み込み、iHeaterに送信し始めます。

## 4. ポータルからの手動制御

スライサーなしでもヒーティングを有効にできます。デバイスカードでチャンバー温度を設定し、**START** をクリックしてください。時間フィールドでは、ヒーティングの時間（分単位）を指定できます。時間を `0` のままにすると、**STOP** をクリックするか、オフコマンドを送信するまで、iHeaterは制限なく動作します。

![iHeaterの手動起動](../../img/iheater-link-settings-03.png)

## 5. マクロの動作を確認する

Klipperコンソールで以下を実行してください：

```gcode
M141 S50
```

iHeater Linkは `target=50` を受け取り、iHeaterのヒーティングを有効にする必要があります。

次に、以下を実行してください：

```gcode
M141 S0
```

Targetが `0` になり、iHeater Linkはヒーティングを無効にします。

## 6. スライサーを設定する

スライサーは `VIRTUAL_CHAMBER` について知る必要はありません。標準的なチャンバー温度コマンドを送信する必要があります：

- `M141 S{T}` — 待機せずにチャンバー温度を設定
- `M191 S{T}` — 待機してチャンバー温度を設定

Klipperマクロはこれらのコマンドをインターセプトし、値を `VIRTUAL_CHAMBER.target` に記録します。

### OrcaSlicer / Bambu Studio

チャンバー温度はフィラメントプロファイルで設定されます：

```text
Filament Settings -> Temperatures -> Chamber temperature
```

例えば：

- ABS / ASA：`40-50 °C`
- PLA：`0 °C`

Gコードの開始を確認してください。次のような行が表示されるはずです：

```gcode
M141 S45
```

### PrusaSlicer / SuperSlicer

プロファイルにチャンバー温度フィールドがある場合、これを使用してください。フィールドがない場合は、Start Gコードに手動でコマンドを追加してください：

```gcode
M141 S45 ; chamber temperature for this filament
```

## 7. 印刷終了時に必ずヒーティングをオフにする

印刷終了時、targetは自動的にリセットされません。End Gコードに以下を追加してください：

```gcode
CLEAR_VIRTUAL_CHAMBER
```

または：

```gcode
M141 S0
```

これにより、`VIRTUAL_CHAMBER.target` が `0` にリセットされ、その後iHeater Linkがアイヒーターをオフにします。
