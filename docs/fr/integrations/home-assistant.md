# Connexion d'iHeater Link à Home Assistant

iHeater Link publie l'appareil dans Home Assistant via MQTT Discovery : HA crée automatiquement une carte avec des capteurs réels et des éléments de contrôle (température cible, durée, mode IDLE/DRYING/STORAGE).

!!! note
    L'appareil **n'apparaîtra pas** dans `Settings → Devices & services → Discovered`. iHeater Link utilise MQTT Discovery, pas UPnP/zeroconf. Une intégration **MQTT** doit **déjà être ajoutée** dans Home Assistant, pointant vers votre broker.

## Prérequis

1. Un broker MQTT (par exemple le module complémentaire Mosquitto) doit être en cours d'exécution dans HA ou accessible via le réseau.
2. L'intégration **MQTT** doit être ajoutée dans HA avec le broker configuré.
3. iHeater Link doit avoir reçu via le portail la commande `link_integration {type:"ha"}` et avoir établi la connexion avec le même broker.

## Étape 1. Ouvrir les paramètres

Dans le menu latéral de Home Assistant, en bas, cliquez sur **Settings**.

![Settings dans le menu latéral](../../img/HA-integration-01.png)

## Étape 2. Accéder à Devices & services

Dans la liste des sections de paramètres, sélectionnez **Devices & services**.

![Devices & services](../../img/HA-integration-02.png)

## Étape 3. Ouvrir l'intégration MQTT

Dans la liste des intégrations, trouvez la carte **MQTT**. Sous le nom se trouve un compteur des appareils connectés.

![MQTT dans la liste des intégrations](../../img/HA-integration-03.png)

## Étape 4. Trouver l'appareil iDryer

Sur la page d'intégration, dans la section **Services**, développez le nœud du broker (`127.0.0.1` ou l'adresse de votre broker). Sous celui-ci se trouvent les appareils iDryer énumérés avec leurs numéros de série de la forme `DEVICE_*`.

![Appareils MQTT](../../img/HA-integration-04.png)

Cliquez sur l'appareil souhaité.

## Étape 5. Gestion et état

La page de l'appareil contient deux blocs :

- **Controls** — éléments de contrôle :
  - `iDryer U1 duration` — durée en minutes
  - `iDryer U1 mode control` — mode (`IDLE` / `DRYING` / `STORAGE`)
  - `iDryer U1 target temp` — température cible (curseur)
- **Sensors** — valeurs réelles. La composition dépend du type d'appareil (`Config` détermine les capteurs publiés) :
  - iHeater Link : `heater_power`, `mode`, `alerts`
  - Storage Link : même chose plus `temperature`, `humidity`

![Page de l'appareil dans HA](../../img/HA-integration-05.png)

Pour démarrer le chauffage :

1. Définissez la température cible avec le curseur.
2. Définissez la durée.
3. Sélectionnez le mode `DRYING` ou `STORAGE` dans le sélecteur.

Pour arrêter — basculez le sélecteur vers `IDLE`.

!!! note
    Les valeurs `target temp` et `duration` sont d'abord sauvegardées sur l'appareil en tant que « réservées », le démarrage réel a lieu lors de la sélection du mode. Cela permet de définir les paramètres dans n'importe quel ordre et de démarrer en une seule action.

## Fonctionnement interne

- **Discovery** (création d'une entity dans l'interface HA avec les bonnes icônes) — publié automatiquement lors de la connexion au broker HA. La composition est déterminée par les drapeaux `Config.hasXxx` — les capteurs manquants n'apparaissent pas en tant que fantômes.
- **State** (valeurs actuelles) — publié dans les rubriques HA toutes les 5 secondes en parallèle avec la publication sur le portail.
- **Commands** (`set_temp` / `set_duration` / `set_mode`) — viennent de HA → broker MQTT → appareil → assemblés dans `Request` et suivent le même chemin que les commandes du portail. Aucune branche spécifique à HA dans le code du produit.

## Dépannage

| Symptôme | À vérifier |
|---------|-------------|
| L'appareil n'apparaît pas dans HA | Sur l'appareil dans le portail — `Home Assistant → Activé : oui`. Le champ `ha.state` dans `integrations/status` doit être `online`. |
| Discovery est publié mais la carte est vide | Attendez 5–10 secondes après la première connexion. Si les valeurs n'apparaissent pas — vérifiez que le broker MQTT ne perd pas les messages retained. |
| Les boutons de contrôle ne réagissent pas | Vérifiez le `command_topic` de Discovery — la rubrique doit correspondre à `idryer/{serial}/U1/set_mode` et ainsi de suite. |
| Capteurs fantômes avec valeur Inconnu | Ancien retained Discovery d'une version précédente du firmware. Après la mise à jour, attendez le prochain cycle de publication Discovery ou nettoyez le retained : `mosquitto_pub -t 'homeassistant/.../config' -n -r`. |
