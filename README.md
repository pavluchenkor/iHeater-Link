<div align="center">

<img src="docs/img/iDryer_logo_small.png" width="220" alt="iDryer">

# iHeater Link

**Wi-Fi module for iHeater. The printer sets the chamber temperature, and the chamber heats itself.**

[![Docs](https://img.shields.io/badge/docs-idryer.org-e7352c)](https://docs.idryer.org/en/projects/iheater/link/) [![Telegram](https://img.shields.io/badge/Telegram-iDryer-2ca5e0)](https://t.me/iDryer) [![Discord](https://img.shields.io/badge/Discord-join-5865f2)](https://discord.gg/jGce5eeHHz) [![License](https://img.shields.io/badge/license-Apache--2.0-blue)](LICENSE)

<img src="docs/img/iHeater_promo.png" width="480" alt="iHeater">

</div>

---

## What it is

An ESP32 board that connects the [iHeater](https://github.com/pavluchenkor/iHeater) chamber heater to your printer over Wi-Fi. The printer reports the chamber temperature it needs, and iHeater Link passes that to the heater.

The link to the heater is a single signal wire. Link handles Wi-Fi and integrations. Heating and safety stay with iHeater.

## The problem it solves

An active chamber is what ABS, PA, PC and other engineering filaments require.

iHeater was built for Klipper, where the heater wires in directly and lives in the printer configuration. But more and more machines ship with proprietary firmware you cannot get into: Bambu Lab, Creality, FlashForge and others. That left a choice between bad options. Crack the firmware, replace it, or print without a chamber.

iHeater Link removes that choice. It works on top of what the printer already exposes: Moonraker with a ready-made macro on Klipper, MQTT on Bambu Lab. You never touch printer firmware, and you never run a wire to it.

After that it runs itself. The printer reports what it is printing and with which material, the chamber comes up to temperature, and heating stops when the print ends.

## Who it is for

- Your printer has no active chamber, and you want to print ABS, PA or PC.
- You want a predictable result: no warping, no parts lifting off the bed, no cracks between layers.
- You need parts that are strong, not just clean: even shrinkage and real interlayer strength.
- You print in batches, where one reprint costs more than the heater.

Printers with an active chamber cost several times more, even though mechanically they are often no different from ordinary ones. iHeater with iHeater Link gives you the same chamber, the same workflow and the same part quality on the printer already sitting on your bench.

Off-the-shelf modules fall into two camps. The cheap ones are built around generic thermal controllers, with all the electronics living inside the hot chamber. The ones from large manufacturers are properly engineered and priced to match. iHeater fills the gap: an engineering-grade chamber without the markup, with firmware you can tune to your own needs.

## Why the board sits outside

A working chamber holds 60 °C and above. An ESP32 overheats in that environment, and sustained heat breaks the Wi-Fi link. The printer enclosure often acts as a shield as well.

A single signal wire settles this. The ESP32 mounts outside, and the only board left inside is iHeater, which is designed for high temperature. Wire length is limited only by sensible loading of the line. Tens of centimetres cause no trouble.

## What it looks like

<img src="docs/img/iHeaterLink.png" width="640" alt="ESP32-C3 Super Mini wired to iHeater">

Three wires: power, ground and signal.

| ESP | iHeater | Purpose |
|---|---|---|
| `5V` | `5V` | controller power |
| `GND` | `GND` | common ground |
| `GPIO3` | signal input | pulsed setpoint |

> **Never connect or disconnect wires while power is applied.**

Power reaches the ESP over USB-C, and the ESP feeds iHeater over the 5 V line. This is the simplest wiring. iHeater can be powered separately if you need it: Link does not depend on the power scheme.

## Portal and app

The device appears at [portal.idryer.org](https://portal.idryer.org) and in the mobile app: readings, history, settings.

Printer integrations are configured in the same place. Bambu Lab, Moonraker and Home Assistant are enabled from the device card. You can also turn heating on or off by hand there, when automation is not enough.

<div align="center">

<img src="docs/img/portal1.png" width="420" alt="Portal: device overview"> <img src="docs/img/portal2.png" width="420" alt="Portal: spool tracking">

</div>

<div align="center">

<img src="docs/img/app-01-home.png" width="230" alt="App: home screen"> <img src="docs/img/app-02-device.png" width="230" alt="App: device card"> <img src="docs/img/app-03-session.png" width="230" alt="App: session report">

</div>

- [iDryer on the App Store](https://apps.apple.com/app/idryer/id6760609044)
- [iDryer on Google Play](https://play.google.com/store/apps/details?id=org.idryer.mobile)

## Place in the ecosystem

| Layer | What it does | Repository |
|---|---|---|
| Heater | Chamber heating, fan, safety | [iHeater](https://github.com/pavluchenkor/iHeater) |
| Heater firmware | Standalone mode, pulse protocol | [iHeater Standalone Firmware](https://github.com/pavluchenkor/iHeater-Standalone-Firmware) |
| Link | Wi-Fi, portal, integrations — **this repository** | iHeater-link |
| Protocol | MQTT and UART contract shared by every device | [idryer-core](https://github.com/pavluchenkor/idryer-core) |
| Cloud | Portal, app, integrations | [portal.idryer.org](https://portal.idryer.org/) |

## What you need (BOM)

| Component | Qty | Notes | Where to get it |
|---|---|---|---|
| ESP32-C3 Super Mini board | 1 | any ESP32-C3 or ESP32-S3 with a free GPIO will do | [link](https://es.aliexpress.com/w/wholesale-es32-c3-super-mini.html) |
| iHeater controller | 1 | with `iheater_revX_X_pulse` firmware | [store.idryer.org](https://store.idryer.org/) |
| Wires | 3 | power, ground, signal | — |
| USB Type-C cable | 1 | power and first flash, any cable will do | — |

Verified boards: ESP32-C3 Super Mini, ESP32-C3 DevKitM-1, Seeed XIAO ESP32-S3, Waveshare ESP32-S3-Zero.

## Difficulty and cost

| | |
|---|---|
| Soldering | not required if you use ready-made wires |
| 3D printing | not required |
| Hazardous voltage | none, only 5 V on the Link side |
| Skills | connect three wires and flash the firmware from a browser |
| Time | about half an hour |
| Board cost | ~$1.5 |

## Quick start

1. **Connect three wires** to iHeater: `5V`, `GND`, and `GPIO3` to the signal input. Power must be off while you do this.
2. **Flash from your browser** at [install.idryer.org](https://install.idryer.org/).
3. **Connect to Wi-Fi** and bind the device at [portal.idryer.org](https://portal.idryer.org/).
4. **Enable a printer integration**: Moonraker, Bambu Lab or Home Assistant. For Moonraker, add the ready-made macro to the printer. Without it the setpoint never reaches Link.
5. **Start a print** with a chamber temperature set, and heating turns on by itself.

Full instructions are in the [documentation](https://docs.idryer.org/en/projects/iheater/link/).

## Integrations

| Printer | What you do on the printer | How it works |
|---|---|---|
| **Bambu Lab** | nothing | Link connects to the printer over MQTT on the local network, sees the active filament and the start of the print, and takes the chamber temperature from the material table |
| **Klipper** | one `virtual_chamber.cfg` file with a couple of macros, included from `printer.cfg` | the macros intercept the standard `M141` and `M191`, and Link reads the setpoint over the network |
| **Home Assistant** | nothing | the setpoint comes from your smart home automations |

Klipper needs neither root access nor a change to printer firmware. Access to user macros is enough. That is the route for locked Klipper printers: Creality, FlashForge.

The slicer needs no changes either. `M141` and `M191` are the standard chamber temperature commands, and the slicer emits them on its own. All you have to do is set a chamber temperature in the material profile.

Step-by-step setup is in the [documentation](https://docs.idryer.org/en/projects/iheater/link/).

## Status

Working firmware. All three integrations are in service: Bambu Lab and Klipper are verified on hardware, and the printer to setpoint to heating chain has been exercised from print start through chamber shutdown. Home Assistant is enabled from the device card.

Developed alongside the iHeater firmware and the portal.

## Boards and protocol

The firmware builds for any ESP32 with a free GPIO for the signal, in the worst case with minor modifications. The ready-made build environments are listed in `platformio.ini`.

The protocol is shared across the ecosystem and defined in `idryer-core` (`contracts/mqtt_contract.yaml`). Changes are made there and propagate to the firmwares and the portal by generation. The protocol is not edited in this repository.

## License

Code: [Apache License 2.0](LICENSE), [NOTICE](NOTICE).

The iDryer name is not covered by the license. See [TRADEMARKS.md](https://github.com/pavluchenkor/idryer-core/blob/main/TRADEMARKS.md).

The hardware design is licensed separately.

## Help

- [Telegram](https://t.me/iDryer)
- [Discord](https://discord.gg/jGce5eeHHz)
- [Documentation](https://docs.idryer.org/en/projects/iheater/link/)

Guides and teardowns on the channel: [YouTube](https://www.youtube.com/@iDryerProject) · [Rutube](https://rutube.ru/channel/34401569/)

## Contributing

Built it on a different board, tested the integration with your printer, found a discrepancy? Open an issue or send a pull request.

## Next

[Flash the board from your browser](https://install.idryer.org/).
