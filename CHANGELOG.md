# Changelog

Notable changes to the `iHeater-link` firmware — the link module between the iHeater and the portal. Russian version: [CHANGELOG.ru.md](CHANGELOG.ru.md).

## Types of changes

- **Added** — for new features.
- **Changed** — for changes in existing functionality.
- **Deprecated** — for soon-to-be removed features.
- **Removed** — for now removed features.
- **Fixed** — for any bug fixes.
- **Security** — in case of vulnerabilities.

## [3.0.0] — 2026-09-23

### Added

- **Firmware updates over the air.** The link module updates itself. The heater's own firmware is not updated over the air: it is connected by a single signal wire and is flashed over USB.
- **Portal binding without a PIN.** The device receives its key on connection and is addressed in the cloud by its own identifier.
- **Unbinding from the portal.** A portal command wipes the key and the device returns to the pairing state.
- **Chamber temperature from the active integration.** The target sent by the printer is visible in the portal and in the app.
- **Bambu Lab print progress in the device status.**
- **Flash layout without a filesystem.** Each firmware partition is 1984 KB instead of 1280 KB: the firmware does not use a filesystem.

### Changed

- **The local network keeps control even when the cloud is off.** The "ignore external commands" setting now blocks the portal only. The app on the same network keeps controlling the heater.
- The working mode is called "Heating" instead of "Drying".
- The firmware moved to the shared `idryer-core` library: portal link, binding and protocol are the same across the ecosystem.
- **Better link stability on ESP32-C3 Super Mini.** Wi-Fi transmit power is limited on these boards.
- **A setting change is confirmed by a patch instead of resending the whole menu.** The portal receives the single changed value instead of three and a half kilobytes, and the device does not assemble the entire menu on every edit.
- **The whole menu is sent when the device comes online.** Previously it was sent only on the portal's request, and the snapshot on the broker went stale.
- Telemetry and status periods are no longer set in the firmware — the core takes them from the contract. The old values matched the contract but duplicated it.
- **Print progress and time left on the heating card.** The device reports the print percentage from Bambu and Moonraker, and the remaining time from Bambu. Previously only Bambu's percentage was sent, and the remaining time was parsed but never published.
- **The device declares its integrations.** All three are compiled into the heater — Home Assistant, Bambu Lab and Moonraker; the portal and the app draw the buttons from that list.

### Removed

- The "PORTAL" menu section with the "LINK" item. Portal binding does not involve the menu.
- **The "CONNECTIONS" menu section with the Bambu, Moonraker and Home Assistant toggles.** A toggle could only switch an integration on and off, while the broker address, printer serial and keys are set exclusively in the integrations section of the portal and the app — there was essentially nothing for the menu to switch on. On top of that the menu and the integrations section changed the same state independently and drifted apart. There is a single path now: the integrations section.

### Fixed

- **The device rebooted when an integration was switched on** (Moonraker, Bambu Lab, Home Assistant). The settings survived, but the link dropped and the portal showed the device offline. The cause was assembling the whole menu in response to the command; a patch goes out now, which is an order of magnitude lighter.
- **The heater's menu and settings are visible over the local network.** Previously the configuration went to the cloud only, and the app on the local network did not receive it. Values edited over the local network now show up too.
- Critical heater errors reach the portal as critical rather than as ordinary events.

## [2.x]

A series of trial builds. The direction was closed, the work carried over into 3.0.0.
