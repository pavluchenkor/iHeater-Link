# Home Assistant

iHeater Link se publie dans Home Assistant via **MQTT Discovery** : HA crée lui-même les entités — température, puissance de chauffe, champs de lancement et boutons. Le portail n'est pas nécessaire, tout passe par votre broker MQTT.

Ci-dessous : l'activation de l'intégration, la vérification et une disposition de carte prête à l'emploi, pour que l'appareil ait un aspect soigné et non celui d'une liste d'entités.

![Carte iHeater Link dans Home Assistant](../../img/iheater-ha-card.png)
*La température de la chambre, la puissance de chauffe et le lancement de la chauffe dans un seul bloc.*

!!! note
    L'appareil **n'apparaîtra pas** dans `Settings → Devices & services → Discovered` : il s'agit de MQTT Discovery, pas d'UPnP/zeroconf. L'intégration **MQTT** doit être ajoutée au préalable dans Home Assistant.

## Prérequis

1. Un broker MQTT : le module complémentaire **Mosquitto broker** dans Home Assistant ou tout autre broker de votre réseau.
2. L'intégration **MQTT** ajoutée dans Home Assistant et pointant vers ce broker.
3. iHeater Link présent sur le réseau et `Online` sur le portail.

!!! info "iHeater Link est le module de communication du contrôleur iHeater ; flashez le contrôleur avec le micrologiciel [iheater_revX_X_pulse](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases)."

## Étape 1. Activer l'intégration sur l'appareil

Ouvrez l'appareil sur [portal.idryer.org](https://portal.idryer.org/) et trouvez le bloc **Intégrations** → **Home Assistant**.

| Champ | Que saisir |
|---|---|
| Host | l'adresse du broker dans votre réseau, par exemple `192.168.1.27` |
| Port | le port du broker, généralement `1883` |
| Username / Password | les identifiants du broker, s'il les exige |
| Discovery prefix | `homeassistant`, si vous ne l'avez pas modifié dans les paramètres de HA |
| Activé | la case à cocher — sinon l'appareil ne se connectera pas au broker |

Les paramètres sont transmis directement à l'appareil via le réseau local — le portail ne les conserve pas. Home Assistant s'active par son propre interrupteur et ne gêne pas les intégrations d'imprimante : Bambu Lab et Moonraker se choisissent séparément, et une seule des deux fonctionne à la fois.

![Fenêtre Home Assistant dans le bloc « Intégrations » du portail](../../img/iheater-ha-portal-integration.png)
*L'adresse du broker, le port et la case « Activé » — c'est tout ce dont l'appareil a besoin.*

## Étape 2. Trouver l'appareil dans Home Assistant

Dans le menu latéral, en bas, cliquez sur **Settings**.

![Settings dans le menu latéral](../../img/HA-integration-01.png)

Sélectionnez **Devices & services**.

![Devices & services](../../img/HA-integration-02.png)

Trouvez la carte **MQTT**. Sous son nom figure le compteur des appareils connectés.

![MQTT dans la liste des intégrations](../../img/HA-integration-03.png)

Dans la section **Services**, développez le nœud du broker. Les appareils iDryer y figurent sous des numéros de série de la forme `DEVICE_*`.

![Appareils MQTT](../../img/HA-integration-04.png)

Ouvrez l'appareil : HA affiche déjà les relevés et les éléments de contrôle.

![Page de l'appareil dans HA](../../img/HA-integration-05.png)

## Étape 3. Composer la carte

HA dispose les entités lui-même, ce qui donne une longue liste. La disposition prête à l'emploi place les relevés en haut et le lancement de la chauffe dans un bloc distinct.

1. `Settings` → `Dashboards` → **Add dashboard** → un tableau de bord vide, ouvrez-le.
2. Coin supérieur droit → crayon (**Edit**) → menu « ⋮ » → **Raw configuration editor**.
3. Collez le contenu ci-dessous et enregistrez.

La disposition est prévue pour un tableau de bord de type `sections`.

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

![Raw configuration editor avec la disposition collée](../../img/iheater-ha-raw-editor.png)
*La même disposition dans l'éditeur de configuration du tableau de bord.*

L'ordre de lancement est le même que dans l'application : on définit d'abord la température et la durée, puis on appuie sur **Démarrer le chauffage**. Le bouton **Stop** coupe la chauffe.

## Si les noms des entités ne correspondent pas

La disposition est prévue pour des identifiants standard de la forme `sensor.iheater_link_temperature`. Si la carte affiche « Entity not found », consultez les vôtres : `Settings` → `Devices & services` → **MQTT** → votre appareil → liste des entités, — et remplacez le préfixe dans la disposition par le vôtre.

## Sous le capot

- L'appareil déclare lui-même la composition de ses entités — à partir de la description de sa carte : relevés, champs de paramètres et boutons d'action. Ce que l'appareil n'a pas n'apparaît pas dans Home Assistant.
- Les valeurs sont publiées dans MQTT avec la télémétrie habituelle ; HA les reçoit en temps réel.
- L'appui sur un bouton dans HA parvient à l'appareil comme la même action que depuis le portail ou l'application — il n'y a pas de logique « pour Home Assistant » à part dans le micrologiciel.

## Diagnostic

| Symptôme | À vérifier |
|---|---|
| L'appareil n'apparaît pas dans HA | Sur le portail, la case « Activé » de l'intégration Home Assistant est cochée, l'adresse et le port du broker sont corrects. L'appareil doit être `Online`. |
| Il apparaît, mais les valeurs sont `Unknown` | Attendez un cycle de télémétrie. Si rien n'arrive ensuite — le broker ne conserve pas les messages retained, ou l'appareil ne s'y est pas connecté. |
| Pas de température de chambre | Le capteur n'est pas branché au contrôleur iHeater : sans lui, l'appareil ne publie pas cette valeur. |
| Les boutons ne réagissent pas | Vérifiez que le broker autorise la publication dans les topics `idryer/#` et que le journal de l'appareil ne contient pas d'erreurs d'autorisation. |
| Entités fantômes avec la valeur `Unknown` | Des messages retained d'un ancien micrologiciel subsistent. Nettoyez-les : `mosquitto_pub -h <broker> -t 'homeassistant/<...>/config' -n -r`. |
| Bambu ou Moonraker a disparu en même temps que Home Assistant | Home Assistant n'y est pour rien — il s'active séparément. Vérifiez le choix de l'intégration d'imprimante : une seule des deux fonctionne. |
