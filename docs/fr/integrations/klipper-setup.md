# Configuration de Klipper pour iHeater Link

## À quoi sert cela

Cette fonctionnalité est destinée aux imprimantes Klipper avec un système verrouillé : Creality, Qidi, Flashforge et autres modèles modernes, où l'utilisateur ne peut pas compiler et installer le firmware iHeater pour travailler directement avec Klipper.

Les fichiers de configuration Klipper habituels et les macros G-code personnalisées sont souvent accessibles. Par conséquent, iHeater Link utilise un chemin plus simple : il se connecte au même réseau Wi-Fi que l'imprimante, lit les variables de la macro Klipper personnalisée et transmet la température cible au contrôleur iHeater.

Du côté de l'imprimante, vous n'avez besoin d'ajouter que quelques macros G-code. Ils acceptent les commandes de température de chambre standard `M141` et `M191` et stockent la température cible dans `VIRTUAL_CHAMBER`.

Le schéma final :

```text
Trancheur / G-code -> M141 S50 -> Macros Klipper VIRTUAL_CHAMBER.target=50
                                      |
                                      v
Imprimante sur le réseau local <- Wi-Fi <- iHeater Link -> ligne de signal -> iHeater
```

L'utilisateur n'a pas besoin d'obtenir l'accès root ou de modifier le firmware interne de l'imprimante. Il suffit d'avoir accès aux macros Klipper personnalisées.

## Que vous obtiendrez

```text
M141 S50 -> target = 50 -> iHeater Link active le chauffage
M141 S0  -> target = 0  -> iHeater Link désactive le chauffage
```

De nombreuses imprimantes modernes ont déjà un capteur de température à l'intérieur de la chambre. Si ce capteur est prévu par le fabricant et visible dans la configuration Klipper, il peut être utilisé pour transmettre la température réelle au portail et à iHeater Link. S'il n'y a pas de capteur, iHeater Link pourra toujours contrôler le chauffage selon la température cible.

## 1. Ajoutez le fichier de macros

Créez un fichier `virtual_chamber.cfg` dans la configuration Klipper et incluez-le depuis `printer.cfg` :

```ini
[include virtual_chamber.cfg]
```

Contenu de `virtual_chamber.cfg` :

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

Après enregistrement, redémarrez Klipper ou exécutez `RESTART`.

## 2. Connectez optionnellement le capteur de chambre

Ouvrez la configuration de l'imprimante et vérifiez s'il y a un objet qui ressemble à un capteur de température de chambre. Chez différents fabricants et configurations, il peut avoir différents noms, par exemple :

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

S'il existe un tel objet, vous pouvez transmettre à iHeater Link la température réelle. Ajoutez le bloc ci-dessous à `virtual_chamber.cfg` et remplacez `heater_generic chamber` par le nom de l'objet dans votre configuration :

```ini
[delayed_gcode UPDATE_VIRTUAL_CHAMBER_TEMP]
initial_duration: 1.0
gcode:
  {{ "{%" }} set t = printer["heater_generic chamber"].temperature|float {{ "%}" }}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=temperature VALUE={t}
  SET_GCODE_VARIABLE MACRO=VIRTUAL_CHAMBER VARIABLE=has_sensor VALUE=1
  UPDATE_DELAYED_GCODE ID=UPDATE_VIRTUAL_CHAMBER_TEMP DURATION=2.0
```

Par exemple, si votre capteur est décrit comme `[temperature_sensor enclosure]`, la ligne de lecture de température doit faire référence à `printer["temperature_sensor enclosure"].temperature`.

S'il n'y a pas de capteur ou si vous n'êtes pas sûr, ignorez cette étape. Pour contrôler le chauffage, `target` transmis par les macros `M141` et `M191` suffit.

## 3. Activez l'intégration Klipper dans iHeater Link

Dans le portail, ouvrez l'appareil iHeater Link et appuyez sur **MOONRAKER** dans le bloc **Device Info**. Dans l'interface, ce nom est utilisé pour les imprimantes Klipper.

![Sélection de Moonraker dans Device Info](../../img/iheater-link-settings-01.png)

Ensuite, cliquez sur l'icône d'engrenage dans la fiche de l'appareil, ouvrez les paramètres de connexion et activez **MOONRAKER**.

![Paramètres de l'appareil](../../img/iheater-link-settings-03.png)

![Activation de Moonraker dans les paramètres de l'appareil](../../img/iheater-link-settings-04.png)

Retournez aux paramètres **MOONRAKER**, spécifiez l'adresse IP de l'imprimante et enregistrez les paramètres.

![Retour aux paramètres Moonraker](../../img/iheater-link-settings-05.png)

![Paramètres Moonraker](../../img/iheater-link-settings-02.png)

Généralement, les paramètres suivants suffisent :

- Host : adresse IP de l'imprimante sur le réseau local ;
- Port : `7125` ;
- API key : laisser vide si l'imprimante ne l'exige pas ;
- Use SSL (wss) : désactivé pour une connexion locale normale ;
- Poll interval : `1000`.

Après enregistrement, iHeater Link commencera à lire `VIRTUAL_CHAMBER.target` depuis Klipper et le transmettra à iHeater.

## 4. Contrôle manuel depuis le portail

Le chauffage peut être activé sans trancheur : définissez la température de la chambre dans la fiche de l'appareil et appuyez sur **START**. Dans le champ d'heure, vous pouvez spécifier la durée de chauffage en minutes. Si vous laissez le temps à `0`, iHeater fonctionnera sans limite de temps, jusqu'à ce que vous appuyiez sur **STOP** ou envoyiez une commande d'arrêt.

![Démarrage manuel du iHeater](../../img/iheater-link-settings-03.png)

## 5. Vérifiez le fonctionnement des macros

Dans la console Klipper, exécutez :

```gcode
M141 S50
```

iHeater Link devrait recevoir `target=50` et activer le chauffage du iHeater.

Ensuite, exécutez :

```gcode
M141 S0
```

Target devient `0`, et iHeater Link désactive le chauffage.

## 6. Configurez le trancheur

Le trancheur n'a pas besoin de connaître `VIRTUAL_CHAMBER`. Il doit envoyer les commandes de température de chambre standard :

- `M141 S{T}` — définir la température de la chambre sans attendre ;
- `M191 S{T}` — définir la température de la chambre en attendant.

Les macros Klipper intercepteront ces commandes et écriront la valeur dans `VIRTUAL_CHAMBER.target`.

### OrcaSlicer / Bambu Studio

La température de la chambre est définie dans le profil de filament :

```text
Filament Settings -> Temperatures -> Chamber temperature
```

Par exemple :

- ABS / ASA : `40-50 °C` ;
- PLA : `0 °C`.

Vérifiez le début du G-code. Une ligne ressemblant à ceci doit apparaître :

```gcode
M141 S45
```

### PrusaSlicer / SuperSlicer

Si le profil a un champ de température de chambre, utilisez-le. Si le champ n'existe pas, ajoutez la commande manuellement dans Start G-code :

```gcode
M141 S45 ; chamber temperature for this filament
```

## 7. N'oubliez pas de désactiver le chauffage à la fin de l'impression

À la fin de l'impression, la target n'est pas réinitialisée automatiquement. Ajoutez à End G-code :

```gcode
CLEAR_VIRTUAL_CHAMBER
```

ou :

```gcode
M141 S0
```

Cela réinitialisera `VIRTUAL_CHAMBER.target` à `0`, après quoi iHeater Link désactivera le iHeater.
