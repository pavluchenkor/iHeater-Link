# iHeater Link — クイックガイド

iHeater Link は、**iHeater** コントローラー用の通信モジュールです。ファームウェアは [iheater_revХ_Х_pulse](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases) に対応しています。ESP32-C3 / ESP32-S3 ベースのボードで、以下の機能を備えています。

- Wi-Fi に接続して iHeater を [portal.idryer.org](https://portal.idryer.org/) に接続します。
- プリンターからカメラの目標温度を取得します。対応する統合:
  **Moonraker (Klipper)**、**Bambu Lab (LAN)**、または
  **Home Assistant**。
- 目標温度をパルス信号に変換し、単一の GPIO を通じて iHeater コントローラーに送信します。

iHeater の制御は「ワイヤード」で行われます。1 つの信号ピン ESP → iHeater の信号入力。Wi-Fi と統合は Link の責務、加熱と安全性は iHeater の責務です。

単一ワイヤー接続は Link の配置に制限を課しません。ESP ボードを熱カメラの外に移動できます。これにより以下が回避されます。

- カメラが 60°C 以上で動作している場合のチップと周辺機器の過熱。
- 長時間加熱時の無線セクションの熱的フリーズと Wi-Fi セッション喪失。
- 加速度的な劣化。

カメラ内には高温動作用に設計された iHeater のみが残ります。ESP への信号ケーブルの長さは、線の合理的な負荷によってのみ制限されます (数十センチメートル - 特に制限なし)。

## サポートされるボード

| ボード                       |       |
| ---------------------------- | ----- |
| ESP32-C3 Super Mini          | `✅`  |
| ESP32-C3 DevKitM-1           | `✅`  |
| Seeed XIAO ESP32-S3          | `✅`  |
| Waveshare ESP32-S3-Zero      | `✅`  |

ESP32-C3 または ESP32-S3 ベースの他のボードも、信号出力用に空きの GPIO がある場合は使用できます。メーカーのピンアウト資料を参照してください。

## 接続図

!!! warning "電源が入っている状態でケーブルを接続または切断しないでください。"

電源は USB-C を経由して ESP に供給されます。ESP はさらに 5 V ラインを介して iHeater コントローラーに電力を供給します。これが最も簡単な方法です。必要に応じて、iHeater の電源を別の方法で構成することもできます。Link との通信は電源スキーマの影響を受けません。

![ESP32-C3 Super Mini を iHeater に接続](../img/iHeaterLink.png)

接続 (サポートされるすべてのボードの場合):

| ESP        | iHeater          | 目的              |
| ---------- | ---------------- | --------------------- |
| `5V`       | `5V`             | コントローラー電源   |
| `GND`      | `GND`            | 共通グラウンド           |
| `GPIO3`    | 信号入力  | パルス setpoint   |

### ボードのピンアウト

ESP32-C3 Super Mini:

![ESP32-C3 Super Mini ピンアウト](../img/ESP32-C3-Super-Mini-pinout-low.jpg)

Waveshare ESP32-S3-Zero:

![Waveshare ESP32-S3-Zero ピンアウト](../img/ESP32-C3-ZERO-Waveshare-pinout-low.jpg)

## Web フラッシャーを使用したファームウェア書き込み

Web フラッシャーは [install.idryer.org](https://install.idryer.org/) にあります。

1. Link をコンピューターの USB ポートに接続します。
2. [install.idryer.org](https://install.idryer.org/) を開き、**iHeater Link** デバイスを選択します。
3. ボードバリアントを選択します。
4. **Connect** をクリックし、シリアルポートを選択します (通常は `USB JTAG/serial` または `CH340`)。デバイスが認識されない場合は、ボード上の `BOOT` ボタンを押したまま `RST` を短く押します。
5. **Install** をクリックします。フラッシャーがファームウェアを書き込みます。
6. ファームウェア書き込み完了後、Wi-Fi セットアップウィザードが開きます。

## Wi-Fi 設定

ファームウェア書き込み後、Improv ウィザードがシリアルポートで自動的に開きます。

1. 2.4 GHz ネットワークの SSID とパスワードを入力します。
2. **Connected** ステータスを待ちます。Link のインジケーターライトが青色で「呼吸」モードに切り替わります。

ウィザードが開かない場合は、USB を切断して再接続してください。ファームウェア書き込みを繰り返さず **Connect** を使用します。

!!! note "ESP32 は 2.4 GHz のみをサポートします。5 GHz ネットワークは機能しません。"

## ポータルへのバインド

1. フラッシャーページで **接続して Claim を実行** をクリックします。
2. `START_CLAIM` コマンドがデバイスに送信されます。数秒後、ページに PIN が表示されます。PIN は約 5 分間有効です。
3. [portal.idryer.org](https://portal.idryer.org/) → **デバイスを追加** を開き、PIN を入力します。
4. バインドが成功したら、デバイスがオンラインリストに表示されます。

`CLAIM_ALREADY:DEVICE_…` という応答が返された場合、デバイスは既にこのアカウント、または別のアカウントにバインドされています。その場合、ポータルでデバイスを削除して、バインドを再度実行してください。

## iHeater への接続

1. コントローラーの電源を切ります。
2. ESP を上記の図に従って iHeater に接続します。`5V`、`GND`、`GPIO3` → iHeater の信号入力。
3. ESP の USB に電源を供給します。コントローラーは 5 V ラインから電力が供給されます。

Link ロード後、ポータルとの接続を確立し、選択した統合をアクティブ化し、カメラの目標温度を iHeater に送信し始めます。

## 期待される動作

- LED 1 は常に点灯し、LED 3 は 1 秒に 1 回短く点滅します。これは iHeater Link - iHeater 接続の存在を示しています。
- 接続が失われると、すべての LED が 1 Hz の周波数で点滅します。
- その他のエラーは iHeater Link に関連しており、スタンドアロンファームウェアの表示を繰り返します。

## 診断

デバイスメニューに **DIAGNOSTICS → DIAG LOG** という項目があります。有効にすると、Serial に 1 秒ごとに詳細なレポートが出力されます: Wi-Fi ステータス、MQTT、アクティブな統合、現在のターゲット、コネクタエラー。

診断の詳細については、GitHub のプロジェクトリポジトリで利用可能な idryer-core ライブラリドキュメントを参照してください。

