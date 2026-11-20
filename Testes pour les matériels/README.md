# Tests des matériels

Ce dossier regroupe les essais individuels des composants et des fonctions utilisés pendant le développement d’AutoFlame Sentinel.

| Dossier | Essai |
|---|---|
| `test pour capteur de flamme/` | Lecture du capteur de flamme |
| `test pour capteur fumée MQ-2/` | Lecture du capteur de fumée MQ-2 |
| `test pour capteur gaz MQ-135/` | Lecture du capteur de gaz MQ-135 |
| `test pour capteur temp&hum DHT22/` | Mesure de température et d’humidité avec le DHT22 |
| `test pour l'afficheur LCD/` | Vérification de l’afficheur LCD et de sa bibliothèque |
| `test pour le module buzzer/` | Vérification du buzzer |
| `test pour les Leds/` | Vérification des LED |
| `moteur/` | Essai du moteur |
| `Connécté ESP32 au wifi/` | Test de connexion Wi-Fi de l’ESP32 |
| `Connécté ESP32 au Supabase/` | Essai de communication avec Supabase |
| `Connécté ESP32 au Supabase Test/` | Deuxième essai de communication avec Supabase |

Ouvrir le sketch `.ino` de l’essai souhaité dans l’IDE Arduino et vérifier le câblage et les broches indiqués dans le code avant de lancer le test. Installer les bibliothèques Arduino requises depuis le gestionnaire de bibliothèques de l’IDE, notamment `LiquidCrystal_I2C` pour l’écran LCD.

## Configuration privée

Les sketches Wi-Fi et Supabase utilisent des valeurs de remplacement (`YOUR_WIFI_SSID`, `YOUR_WIFI_PASSWORD`, `https://YOUR_PROJECT.supabase.co` et `YOUR_SUPABASE_ANON_KEY`). Remplacer ces valeurs localement avant utilisation. Ne pas publier de mot de passe Wi-Fi, de clé API ou de clé `service_role` dans le dépôt.
