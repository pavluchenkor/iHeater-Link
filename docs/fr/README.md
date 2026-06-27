# iHeater Link — guide rapide

iHeater Link — un module de communication pour le contrôleur **iHeater** avec le firmware [iheater_revХ_Х_pulse](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases). C'est une carte ESP32-C3 / ESP32-S3 qui :

- Se connecte au Wi-Fi et relie iHeater à [portal.idryer.org](https://portal.idryer.org/).
- Récupère la température cible de la chambre depuis l'imprimante via les
  intégrations **Moonraker (Klipper)**, **Bambu Lab (LAN)** ou
  **Home Assistant**.
- Convertit la température cible en signal pulsé et le transmet au contrôleur iHeater sur une seule broche GPIO.

Le contrôle d'iHeater se fait « par câble » : une broche de signal ESP → entrée de signal iHeater. Le Wi-Fi et les intégrations relèvent de Link, le chauffage et la sécurité relèvent d'iHeater.

La connexion monofil n'impose aucune restriction sur le placement de Link. La carte ESP peut être placée en dehors de la chambre thermique. Cela élimine :

- la surchauffe de la puce et de la périphérie lors du fonctionnement de la chambre à 60+ °C ;
- les blocages thermiques de la section radio et la perte de session Wi-Fi lors du chauffage prolongé ;
- la dégradation accélérée.

À l'intérieur de la chambre, seul iHeater reste, conçu pour fonctionner à des températures élevées. La longueur du fil de signal vers l'ESP est limitée uniquement par la charge raisonnable sur la ligne (des dizaines de centimètres — sans réserve).

## Cartes supportées

| Carte                        |       |
| ---------------------------- | ----- |
| ESP32-C3 Super Mini          | `✅`  |
| ESP32-C3 DevKitM-1           | `✅`  |
| Seeed XIAO ESP32-S3          | `✅`  |
| Waveshare ESP32-S3-Zero      | `✅`  |

Toute autre carte basée sur ESP32-C3 ou ESP32-S3 peut être utilisée si
il y a une broche GPIO libre pour la sortie de signal. Vérifiez le brochage
du fabricant.

## Schéma de connexion

!!! warning "Ne connectez et ne déconnectez jamais les câbles avec l'alimentation sous tension."

L'alimentation est fournie à l'ESP via USB-C. L'ESP, à son tour, alimente le contrôleur iHeater via la ligne 5 V. C'est la variante la plus simple. Si nécessaire, l'alimentation d'iHeater peut être organisée différemment — la communication avec Link ne dépend pas du schéma d'alimentation.

![Connexion de l'ESP32-C3 Super Mini à iHeater](../img/iHeaterLink.png)

Connexions (pour toutes les cartes supportées) :

| ESP        | iHeater          | Fonction              |
| ---------- | ---------------- | --------------------- |
| `5V`       | `5V`             | alimentation du contrôleur   |
| `GND`      | `GND`            | masse commune           |
| `GPIO3`    | entrée de signal  | setpoint pulsé   |

### Brochage des cartes

ESP32-C3 Super Mini :

![Brochage de l'ESP32-C3 Super Mini](../img/ESP32-C3-Super-Mini-pinout-low.jpg)

Waveshare ESP32-S3-Zero :

![Brochage de Waveshare ESP32-S3-Zero](../img/ESP32-C3-ZERO-Waveshare-pinout-low.jpg)

## Programmation via le flasheur web

Le flasheur web est disponible sur [install.idryer.org](https://install.idryer.org/).

1. Connectez Link au port USB de votre ordinateur.
2. Ouvrez [install.idryer.org](https://install.idryer.org/) et sélectionnez l'appareil **iHeater Link**.
3. Sélectionnez la variante de carte.
4. Cliquez sur **Connect**, sélectionnez le port série (généralement `USB JTAG/serial`
   ou `CH340`). Si l'appareil n'est pas détecté, maintenez le bouton `BOOT`
   enfoncé sur la carte et appuyez brièvement sur `RST`.
5. Cliquez sur **Install**. Le flasheur écrit le firmware.
6. Une fois la programmation terminée, l'assistant de configuration Wi-Fi s'ouvre.

## Configuration Wi-Fi

Après la programmation, l'assistant Improv s'ouvre automatiquement dans le port série.

1. Entrez le SSID et le mot de passe de votre réseau 2,4 GHz.
2. Attendez le statut **Connected**. L'indicateur Link passera au
   mode « respiration » bleu.

Si l'assistant ne s'est pas ouvert, débranchez l'USB et reconnectez via
**Connect** sans reprogrammer.

!!! note "L'ESP32 ne supporte que 2,4 GHz. Les réseaux 5 GHz ne fonctionnent pas."

## Appairage au portail

1. Sur la page du flasheur, cliquez sur **Connecter et exécuter Claim**.
2. La commande `START_CLAIM` est envoyée à l'appareil. Après quelques secondes,
   un PIN s'affiche sur la page. Le PIN est valide ~5 minutes.
3. Ouvrez [portal.idryer.org](https://portal.idryer.org/) → **Ajouter
   un appareil** → entrez le PIN.
4. Après l'appairage réussi, l'appareil apparaît dans la liste en ligne.

Si la réponse est `CLAIM_ALREADY:DEVICE_…` — l'appareil est déjà lié à ce compte ou à un autre. Dans ce cas, supprimez l'appareil du portail et répétez l'appairage.

## Connexion à iHeater

1. Coupez l'alimentation du contrôleur.
2. Connectez l'ESP à iHeater selon le schéma ci-dessus : `5V`, `GND`, `GPIO3` →
   entrée de signal iHeater.
3. Alimentez l'USB de l'ESP. Le contrôleur sera alimenté via la ligne 5 V.

Après le démarrage, Link établira une connexion avec le portail, activera
l'intégration sélectionnée et commencera à transmettre la température cible
de la chambre à iHeater.

## Résultat attendu

- La LED 1 s'allume en permanence, la LED 3 clignote brièvement 1 fois par seconde, indiquant la présence de la connexion iHeater Link - iHeater.
- En cas de perte de connexion, toutes les LED clignotent à une fréquence de 1 Hz.
- Les autres erreurs sont liées à iHeater Link et répètent l'indication du firmware autonome.

## Diagnostic

Dans le menu de l'appareil, il y a un élément **DIAGNOSTICS → DIAG LOG**. Lorsqu'il est activé, un rapport détaillé est envoyé au port série une fois par seconde : état du Wi-Fi, MQTT, intégration active, cible actuelle, erreurs des connecteurs.

Pour plus de détails sur les diagnostics, consultez la documentation de la bibliothèque idryer-core, disponible dans le référentiel du projet sur GitHub.
