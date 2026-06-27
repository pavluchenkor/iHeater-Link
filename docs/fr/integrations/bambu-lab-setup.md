# Configuration de Bambu Lab pour iHeater Link

## Pourquoi en avez-vous besoin

iHeater Link peut gérer automatiquement iHeater lors de l'impression sur des imprimantes Bambu Lab. Contrairement au scénario Klipper, il n'est pas nécessaire d'ajouter des macros ou de modifier le G-code sur l'imprimante Bambu. Link se connecte à l'imprimante sur le réseau local et lit l'état d'impression et le filament actif.

Sur les imprimantes Bambu Lab actuelles, il n'est pas possible d'utiliser de manière fiable la transmission de la température de la chambre comme commande de contrôle d'iHeater. Cependant, Link peut déterminer quel filament est actuellement installé dans le plateau actif et si l'imprimante a commencé la préparation à l'impression ou est déjà en cours d'impression. En fonction du type de filament, le micrologiciel sélectionne la température de chambre requise dans le tableau des matériaux et active automatiquement iHeater.

Si le type de filament requis ne figure pas dans la liste des matériaux du micrologiciel, veuillez contacter l'auteur du micrologiciel : le type peut être ajouté dans les versions suivantes.

## Ce que vous obtiendrez

```text
Bambu commence la préparation ou l'impression -> Link détecte le filament actif -> sélectionne la température du tableau des matériaux -> iHeater s'active
```

Lorsque l'impression se termine ou que le scénario actif ne nécessite plus de chauffage, Link désactive iHeater.

## 1. Ouvrez les paramètres de l'appareil

Dans le portail, ouvrez la carte iHeater Link et appuyez sur l'icône d'engrenage.

![Ouverture des paramètres de l'appareil](../../img/iheater-link-settings-bambu-02.png)

## 2. Activez la connexion Bambu

Dans les paramètres de l'appareil, ouvrez le bloc **CONNECTIONS** et activez **BAMBU**. Les autres connexions peuvent rester désactivées si elles ne sont pas utilisées.

![Activation de Bambu dans les paramètres de l'appareil](../../img/iheater-link-settings-bambu-03.png)

## 3. Sélectionnez Bambu Lab sur la page de l'appareil

Revenez à la page de l'appareil et appuyez sur **BAMBU LAB** dans le bloc **Device Info**. Le bouton deviendra actif.

![Sélection de Bambu Lab](../../img/iheater-link-settings-bambu-04.png)

## 4. Entrez les paramètres de connexion

Dans les paramètres **Bambu Lab**, activez l'intégration et remplissez les paramètres de connexion.

![Paramètres de Bambu Lab](../../img/iheater-link-settings-bambu-05.png)

Habituellement, vous avez besoin de ces champs :

- Printer IP : adresse IP de l'imprimante sur le réseau local ;
- Printer serial : numéro de série de l'imprimante ;
- LAN access code : code d'accès du mode LAN ;
- Auto-apply on tag detect : activé si Link doit appliquer automatiquement la température en fonction du filament détecté ;
- Default AMS et Default tray : vous pouvez laisser les valeurs par défaut si vous n'avez pas besoin de sélectionner un AMS ou un plateau spécifique.

L'imprimante et iHeater Link doivent être sur le même réseau local. Le code d'accès LAN et le numéro de série sont extraits des paramètres de l'imprimante Bambu Lab.

## 5. Configurez les températures des matériaux

Ouvrez le bloc **MATERIALS** dans les paramètres de l'appareil. Pour chaque type de filament, vous pouvez définir une température de chambre différente.

![Températures des matériaux](../../img/iheater-link-settings-001-materials.png)

Le micrologiciel contient déjà un large ensemble de types de filaments. Lorsque l'imprimante commence la préparation ou l'impression, Link vérifiera le plateau actif, déterminera le type de filament et utilisera la température de ce tableau. Par exemple, pour PLA, vous pouvez définir une basse température ou désactiver le chauffage, pour ABS et ASA, définir une température plus élevée.

Si le filament actif n'est pas trouvé dans le tableau ou si une température inappropriée est définie pour celui-ci, modifiez la valeur dans **MATERIALS** et enregistrez les paramètres.

## 6. Vérifiez le fonctionnement

Lancez l'impression sur Bambu Lab avec un filament pour lequel une température est définie dans **MATERIALS**. Lorsque l'imprimante passe à la préparation ou à l'impression, iHeater Link doit appliquer automatiquement la température du filament actif et activer iHeater.

Si le chauffage ne s'active pas, vérifiez :

- que la connexion **BAMBU** est activée dans les paramètres de l'appareil ;
- que **BAMBU LAB** est sélectionné dans le bloc **Device Info** ;
- que l'adresse IP, le numéro de série et le code d'accès LAN sont correctement spécifiés ;
- que l'imprimante détecte le plateau actif et le type de filament ;
- qu'une température est définie pour ce type de filament dans **MATERIALS**.
