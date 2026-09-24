# Home Assistant

iHeater Link публикует себя в Home Assistant через **MQTT Discovery**: HA сам создаёт сущности — температуру, мощность нагрева, поля запуска и кнопки. Портал для этого не нужен, всё идёт через ваш MQTT-брокер.

Ниже — включение интеграции, проверка и готовая раскладка карточки, чтобы прибор выглядел аккуратно, а не списком сущностей.

<!-- СКРИНШОТ: итоговая карточка iHeater Link в Home Assistant -->

!!! note
    Устройство **не появится** в `Settings → Devices & services → Discovered`: это MQTT Discovery, а не UPnP/zeroconf. Интеграция **MQTT** в Home Assistant должна быть добавлена заранее.

## Что нужно

1. MQTT-брокер: дополнение **Mosquitto broker** в Home Assistant или любой брокер в вашей сети.
2. В Home Assistant добавлена интеграция **MQTT**, указывающая на этот брокер.
3. iHeater Link в сети и `Online` на портале.

!!! info "iHeater Link — модуль связи для контроллера iHeater; прошейте контроллер прошивкой [iheater_revХ_Х_pulse](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases)."

## Шаг 1. Включить интеграцию на приборе

Откройте устройство на [portal.idryer.org](https://portal.idryer.org/) и найдите блок **Интеграции** → **Home Assistant**.

| Поле | Что вписать |
|---|---|
| Host | адрес брокера в вашей сети, например `192.168.1.60` |
| Port | порт брокера, обычно `1883` |
| Username / Password | учётные данные брокера, если он их требует |
| Discovery prefix | `homeassistant`, если не меняли его в настройках HA |
| Включено | галочка — иначе прибор к брокеру не подключится |

Настройки уходят прямо на прибор по локальной сети — портал их не хранит. Активной может быть только одна интеграция: включение Home Assistant отключит Bambu Lab или Moonraker, если они были включены. Целевую температуру камеры при этом будет задавать не принтер, а вы — из Home Assistant или портала.

<!-- СКРИНШОТ: блок «Интеграции» на портале и окно Home Assistant с полями -->

## Шаг 2. Найти устройство в Home Assistant

В боковом меню внизу нажмите **Settings**.

![Settings в боковом меню](../../img/HA-integration-01.png)

Выберите **Devices & services**.

![Devices & services](../../img/HA-integration-02.png)

Найдите карточку **MQTT**. Под названием — счётчик подключённых устройств.

![MQTT в списке интеграций](../../img/HA-integration-03.png)

В разделе **Services** разверните узел брокера. Приборы iDryer видны под серийными номерами вида `DEVICE_*`.

![Устройства MQTT](../../img/HA-integration-04.png)

Откройте устройство: HA уже показывает показания и элементы управления.

![Страница устройства в HA](../../img/HA-integration-05.png)

## Шаг 3. Собрать карточку

HA раскладывает сущности сам, и получается длинный список. Готовая раскладка ставит показания сверху, а запуск нагрева — отдельным блоком.

1. `Settings` → `Dashboards` → **Add dashboard** → пустой дашборд, откройте его.
2. Правый верхний угол → карандаш (**Edit**) → меню «⋮» → **Raw configuration editor**.
3. Вставьте содержимое ниже и сохраните.

Раскладка рассчитана на дашборд типа `sections`.

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
      name: Температура
      visibility:
      - condition: state
        entity: sensor.iheater_link_temperature
        state_not:
        - unknown
        - unavailable
    - type: tile
      entity: sensor.iheater_link_heater_power
      name: Мощность нагрева
    - type: heading
      heading: Нагрев
      heading_style: subtitle
    - type: tile
      entity: number.iheater_link_heat_temperature
      name: Температура
      features:
      - type: numeric-input
        style: buttons
      features_position: bottom
    - type: tile
      entity: number.iheater_link_heat_duration
      name: Длительность
      features:
      - type: numeric-input
        style: buttons
      features_position: bottom
    - type: tile
      entity: button.iheater_link_heat
      name: Запустить нагрев
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
      name: Стоп
      icon: mdi:stop
      hide_state: true
      tap_action: &id002
        action: perform-action
        perform_action: button.press
        target:
          entity_id: button.iheater_link_stop
      icon_tap_action: *id002
```

<!-- СКРИНШОТ: Raw configuration editor со вставленной раскладкой -->

Порядок запуска такой же, как в приложении: сначала задаются температура и длительность, затем нажимается **Запустить нагрев**. Кнопка **Стоп** выключает нагрев.

## Если имена сущностей не совпали

Раскладка рассчитана на стандартные идентификаторы вида `sensor.iheater_link_temperature`. Если карточка показывает «Entity not found», посмотрите свои: `Settings` → `Devices & services` → **MQTT** → ваше устройство → список сущностей, — и замените префикс в раскладке на свой.

## Что происходит под капотом

- Состав сущностей прибор объявляет сам — из описания своей карточки: показания, поля параметров и кнопки действий. Чего у прибора нет, то в Home Assistant не появляется.
- Значения публикуются в MQTT вместе с обычной телеметрией; HA получает их в реальном времени.
- Нажатие кнопки в HA приходит на прибор как то же действие, что и из портала или приложения, — отдельной логики «для Home Assistant» в прошивке нет.

## Диагностика

| Симптом | Что проверить |
|---|---|
| Устройство не появилось в HA | В портале у интеграции Home Assistant стоит галочка «Включено», адрес и порт брокера верны. Прибор должен быть `Online`. |
| Появилось, но значения `Unknown` | Подождите цикл телеметрии. Если пусто дальше — брокер не хранит retained-сообщения либо прибор к нему не подключился. |
| Нет температуры камеры | Датчик не подключён к контроллеру iHeater: без него прибор эту величину не публикует. |
| Кнопки не срабатывают | Проверьте, что у брокера разрешена публикация в топики `idryer/#`, а у прибора в логе нет ошибок авторизации. |
| Сущности-призраки со значением `Unknown` | Остались retained-сообщения от прежней прошивки. Очистите: `mosquitto_pub -h <брокер> -t 'homeassistant/<...>/config' -n -r`. |
| Включили Home Assistant — отвалился Bambu или Moonraker | Так и есть: активной может быть одна интеграция. |
