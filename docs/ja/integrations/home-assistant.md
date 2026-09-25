# Home Assistant

iHeater Link は **MQTT Discovery** によって Home Assistant に自身を公開します。HA がエンティティ（温度、ヒーター出力、加熱開始用フィールド、ボタン）を自動的に作成します。ポータルは不要で、すべては自分の MQTT ブローカーを経由します。

以下では、連携の有効化、動作確認、そして本機がエンティティの羅列ではなく整った形で表示されるためのカードレイアウトを説明します。

![Home Assistant の iHeater Link カード](../../img/iheater-ha-card.png)
*チャンバー温度、ヒーター出力、加熱の開始を 1 つのブロックにまとめた状態。*

!!! note
    デバイスは `Settings → Devices & services → Discovered` には**表示されません**。これは UPnP/zeroconf ではなく MQTT Discovery だからです。Home Assistant には **MQTT** 連携をあらかじめ追加しておく必要があります。

## 必要なもの

1. MQTT ブローカー: Home Assistant のアドオン **Mosquitto broker**、またはネットワーク内の任意のブローカー。
2. Home Assistant に、そのブローカーを指す **MQTT** 連携が追加されていること。
3. iHeater Link がネットワークに接続され、ポータル上で `Online` であること。

!!! info "iHeater Link は iHeater コントローラー用の通信モジュールです。コントローラーには [iheater_revX_X_pulse](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases) のファームウェアを書き込んでください。"

## ステップ 1. 本機で連携を有効にする

[portal.idryer.org](https://portal.idryer.org/) でデバイスを開き、**連携** → **Home Assistant** のブロックを表示します。

| 項目 | 入力する内容 |
|---|---|
| Host | ネットワーク内のブローカーのアドレス。例: `192.168.1.27` |
| Port | ブローカーのポート。通常は `1883` |
| Username / Password | ブローカーが要求する場合の認証情報 |
| Discovery prefix | HA の設定で変更していなければ `homeassistant` |
| 有効 | チェックを入れる。入れないと本機はブローカーに接続しない |

設定はローカルネットワーク経由で本機に直接送信されます。ポータルは保存しません。Home Assistant は独立したスイッチで有効になり、プリンター連携とは干渉しません。Bambu Lab と Moonraker は別途選択し、同時に動作するのはどちらか一方です。

![ポータルの「連携」ブロックにある Home Assistant のウィンドウ](../../img/iheater-ha-portal-integration.png)
*ブローカーのアドレス、ポート、「有効」のチェック — 本機に必要なのはこれだけです。*

## ステップ 2. Home Assistant でデバイスを探す

サイドバーメニューの下部にある **Settings** をクリックします。

![サイドバーメニューの Settings](../../img/HA-integration-01.png)

**Devices & services** を選択します。

![Devices & services](../../img/HA-integration-02.png)

**MQTT** のカードを探します。名前の下に接続済みデバイスのカウンターがあります。

![連携一覧の MQTT](../../img/HA-integration-03.png)

**Services** セクションでブローカーのノードを展開します。iDryer の機器は `DEVICE_*` 形式のシリアル番号で表示されます。

![MQTT のデバイス](../../img/HA-integration-04.png)

デバイスを開くと、HA にはすでに測定値と操作要素が表示されています。

![HA のデバイスページ](../../img/HA-integration-05.png)

## ステップ 3. カードを作成する

HA はエンティティを自動で配置するため、長い一覧になってしまいます。用意されたレイアウトは、測定値を上部に、加熱の開始を別のブロックとして配置します。

1. `Settings` → `Dashboards` → **Add dashboard** → 空のダッシュボードを作成し、開きます。
2. 右上隅 → 鉛筆アイコン（**Edit**）→ 「⋮」メニュー → **Raw configuration editor**。
3. 以下の内容を貼り付けて保存します。

このレイアウトは `sections` タイプのダッシュボードを前提としています。

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

![レイアウトを貼り付けた Raw configuration editor](../../img/iheater-ha-raw-editor.png)
*ダッシュボードの設定エディタに表示された同じレイアウト。*

起動の手順はアプリと同じです。まず温度と時間を設定し、次に **加熱を開始** を押します。**停止** ボタンで加熱を切ります。

## エンティティ名が一致しない場合

このレイアウトは `sensor.iheater_link_temperature` のような標準的な識別子を前提としています。カードに「Entity not found」と表示される場合は、`Settings` → `Devices & services` → **MQTT** → 対象のデバイス → エンティティ一覧で実際の名前を確認し、レイアウト内のプレフィックスを自分のものに置き換えてください。

## 内部の仕組み

- エンティティの構成は本機が自ら宣言します。自身のカード記述に基づく測定値、パラメータの入力欄、動作ボタンです。本機が持たない機能は Home Assistant にも現れません。
- 値は通常のテレメトリと一緒に MQTT へ publish され、HA はそれをリアルタイムに受信します。
- HA でボタンを押すと、ポータルやアプリからと同じ動作として本機に届きます。ファームウェアに「Home Assistant 専用」のロジックはありません。

## トラブルシューティング

| 症状 | 確認する内容 |
|---|---|
| HA にデバイスが表示されない | ポータルの Home Assistant 連携で「有効」にチェックが入っていること、ブローカーのアドレスとポートが正しいこと。本機は `Online` である必要があります。 |
| 表示されたが値が `Unknown` | テレメトリの周期を待ってください。その後も空のままなら、ブローカーが retained メッセージを保持していないか、本機が接続できていません。 |
| チャンバー温度が表示されない | センサーが iHeater コントローラーに接続されていません。センサーがない場合、本機はこの値を publish しません。 |
| ボタンが反応しない | ブローカーで `idryer/#` トピックへの publish が許可されているか、本機のログに認証エラーが出ていないかを確認してください。 |
| 値が `Unknown` のゴーストエンティティ | 以前のファームウェアの retained メッセージが残っています。次のコマンドで消去します: `mosquitto_pub -h <ブローカー> -t 'homeassistant/<...>/config' -n -r`。 |
| Home Assistant と一緒に Bambu や Moonraker が消えた | Home Assistant は無関係です。別々に有効化されます。プリンター連携の選択を確認してください。動作するのはどちらか一方です。 |
