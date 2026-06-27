# Connecting iHeater Link to Home Assistant

iHeater Link publishes the device to Home Assistant via MQTT Discovery: HA automatically creates a card with real sensors and controls (target temperature, duration, mode IDLE/DRYING/STORAGE).

!!! note
    The device **will not appear** in `Settings → Devices & services → Discovered`. iHeater Link uses MQTT Discovery, not UPnP/zeroconf. The **MQTT** integration must **already be added** in Home Assistant, pointing at your broker.

## Prerequisites

1. An MQTT broker (e.g. Mosquitto add-on) is running on the HA side or reachable over the network.
2. The **MQTT** integration is added in HA and configured against that broker.
3. iHeater Link has received `link_integration {type:"ha"}` from the portal and established a connection to the same broker.

## Step 1. Open settings

In the Home Assistant sidebar, click **Settings** at the bottom.

![Settings in the sidebar](../../img/HA-integration-01.png)

## Step 2. Go to Devices & services

From the settings list pick **Devices & services**.

![Devices & services](../../img/HA-integration-02.png)

## Step 3. Open the MQTT integration

Find the **MQTT** card in the integrations list. Below the name is the connected-device count.

![MQTT in the integrations list](../../img/HA-integration-03.png)

## Step 4. Locate the iDryer device

On the integration page expand the broker entry under **Services** (`127.0.0.1` or your broker's address). All iDryer devices are listed below it by serial number (`DEVICE_*`).

![MQTT devices](../../img/HA-integration-04.png)

Click the device you need.

## Step 5. Control & state

The device page has two blocks:

- **Controls** — input elements:
  - `iDryer U1 duration` — duration in minutes
  - `iDryer U1 mode control` — mode (`IDLE` / `DRYING` / `STORAGE`)
  - `iDryer U1 target temp` — target temperature (slider)
- **Sensors** — real values. Set depends on device type (`Config` decides which sensors are published):
  - iHeater Link: `heater_power`, `mode`, `alerts`
  - Storage Link: same plus `temperature`, `humidity`

![Device page in HA](../../img/HA-integration-05.png)

To start heating:

1. Set the target temperature with the slider.
2. Set the duration.
3. Select `DRYING` or `STORAGE` in the mode selector.

To stop — switch the selector to `IDLE`.

!!! note
    `target temp` and `duration` are first stored on the device as **pending** values; the actual run starts when you pick the mode. This lets you set parameters in any order and trigger with one action.

## Under the hood

- **Discovery** (HA UI entities with proper icons) is published automatically on connect. Set is driven by `Config.hasXxx` flags — missing sensors do not appear as ghosts.
- **State** (current values) is published to HA topics every 5 seconds in parallel with the portal.
- **Commands** (`set_temp` / `set_duration` / `set_mode`) go HA → MQTT broker → device → assembled into a `Request` and routed through the same path as portal commands. No HA-specific branches in product code.

## Troubleshooting

| Symptom | What to check |
|---------|---------------|
| Device does not appear in HA | On the device in the portal: `Home Assistant → Enabled: yes`. The `ha.state` field in `integrations/status` should be `online`. |
| Discovery is published, card is empty | Wait 5–10 seconds after the first connect. If values are still missing — verify the MQTT broker is not dropping retained messages. |
| Control buttons do nothing | Check the `command_topic` from Discovery — it must match `idryer/{serial}/U1/set_mode` and friends. |
| Ghost sensors stuck at Unknown | Stale retained Discovery from a previous firmware. Either wait for the next Discovery publish cycle or clear retained manually: `mosquitto_pub -t 'homeassistant/.../config' -n -r`. |
