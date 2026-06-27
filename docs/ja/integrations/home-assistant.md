# iHeater Link を Home Assistant に接続する

iHeater Link は MQTT Discovery を通じて Home Assistant にデバイスを公開します。HA は自動的に実センサーと制御要素を備えたカードを作成します（目標温度、継続時間、IDLE/DRYING/STORAGE モード）。

!!! note
    デバイスは `Settings → Devices & services → Discovered` に表示されません。iHeater Link は MQTT Discovery を使用しており、UPnP/zeroconf ではありません。Home Assistant には既に MQTT ブローカーを指定した **MQTT** インテグレーションが追加されていなければなりません。

## 準備が必要なもの

1. MQTT ブローカー（例：Mosquitto アドオン）が HA で実行中であるか、ネットワーク経由でアクセス可能である。
2. HA に **MQTT** インテグレーションが追加され、ブローカーが設定されている。
3. iHeater Link がポータルから `link_integration {type:"ha"}` コマンドを受け取り、同じブローカーとの接続を確立している。

## ステップ 1. 設定を開く

Home Assistant のサイドバーメニューの下部にある **Settings** をクリックします。

![サイドバーメニューの Settings](../../img/HA-integration-01.png)

## ステップ 2. Devices & services に移動

設定セクションのリストから **Devices & services** を選択します。

![Devices & services](../../img/HA-integration-02.png)

## ステップ 3. MQTT インテグレーションを開く

インテグレーションのリストから **MQTT** カードを見つけます。名前の下に接続されたデバイスのカウンターがあります。

![インテグレーション一覧の MQTT](../../img/HA-integration-03.png)

## ステップ 4. iDryer デバイスを探す

インテグレーションページの **Services** セクションで、ブローカーノード（`127.0.0.1` またはブローカーのアドレス）を展開します。その下に `DEVICE_*` という形式のシリアルナンバーを持つ iDryer デバイスがリストされています。

![MQTT デバイス](../../img/HA-integration-04.png)

必要なデバイスをクリックします。

## ステップ 5. 制御と状態

デバイスページには 2 つのセクションがあります：

- **Controls** — 制御要素：
  - `iDryer U1 duration` — 分単位の継続時間
  - `iDryer U1 mode control` — モード（`IDLE` / `DRYING` / `STORAGE`）
  - `iDryer U1 target temp` — 目標温度（スライダー）
- **Sensors** — 実値。構成はデバイスのタイプによって異なります（`Config` は公開されるセンサーを決定します）：
  - iHeater Link: `heater_power`, `mode`, `alerts`
  - Storage Link: 上記に加えて `temperature`, `humidity`

![HA のデバイスページ](../../img/HA-integration-05.png)

加熱を開始するには：

1. スライダーで目標温度を設定します。
2. 継続時間を設定します。
3. セレクターで `DRYING` または `STORAGE` モードを選択します。

停止するには、セレクターを `IDLE` に切り替えます。

!!! note
    `target temp` と `duration` の値は最初にデバイスに「保留中」として保存され、実際の開始はモード選択時に行われます。これにより、パラメータを任意の順序で設定し、1 つの操作で開始できます。

## 内部の動作

- **Discovery**（HA UI に正しいアイコンで entity を作成）— HA ブローカーに接続時に自動的に公開されます。構成は `Config.hasXxx` フラグによって決定されます。存在しないセンサーはファントムとして表示されません。
- **State**（現在の値）— 5 秒ごとにポータルへの公開と並行して HA トピックに公開されます。
- **Commands**（`set_temp` / `set_duration` / `set_mode`）— HA → MQTT ブローカー → デバイス → `Request` に集約され、ポータルコマンドと同じパスを通ります。プロダクトコードに HA 固有の分岐はありません。

## 診断

| 症状 | チェック項目 |
|---------|---------------|
| デバイスが HA に表示されない | ポータルのデバイスで `Home Assistant → Enabled: yes` を確認します。`integrations/status` の `ha.state` フィールドは `online` であるべきです。 |
| Discovery は公開されたが、カードが空 | 最初の接続後 5～10 秒待ちます。値が表示されない場合は、MQTT ブローカーが retained メッセージを失っていないか確認してください。 |
| 制御ボタンが応答しない | Discovery から `command_topic` を確認します。トピックは `idryer/{serial}/U1/set_mode` などと一致するはずです。 |
| 不明な値を持つファントムセンサー | 前のファームウェアバージョンからの古い retained Discovery。アップデート後、次の Discovery 公開サイクルを待つか、retained をクリアします：`mosquitto_pub -t 'homeassistant/.../config' -n -r`。 |
