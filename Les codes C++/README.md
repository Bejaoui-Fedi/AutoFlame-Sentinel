# Codes C++ du projet AutoFlame Sentinel

Ce dossier regroupe les versions des sketches Arduino/C++ développées pour le système d’alarme incendie du projet de fin d’études.

| Version | Fichier principal | Contenu |
|---|---|---|
| 0 | `Code C++ (version 0)/C++.txt` | Première version intégrée : DHT22, capteurs MQ-2 et MQ-135, capteur de flamme, LCD, buzzer et LED. |
| 1 | `Code C++ (version 1)/sketch_may19a/sketch_may19a.ino` | Itération intermédiaire du sketch. |
| 2 | `Code C++ (version 2)/sketch_may26a/sketch_may26a.ino` | Version intégrée avec connexion Wi-Fi et communication Supabase. |
| 3 | `Code C++ (version 3)/sketch_jun02c/sketch_jun02c.ino` | Itération la plus récente fournie, avec connexion Wi-Fi et communication Supabase. |

## Préparation

1. Installer le support de carte ESP32 dans l’IDE Arduino.
2. Ouvrir le sketch de la version souhaitée. Pour la version 0, copier le contenu de `C++.txt` dans un nouveau sketch Arduino.
3. Installer les bibliothèques `DHT sensor library`, `Adafruit Unified Sensor` et `LiquidCrystal_I2C` depuis le gestionnaire de bibliothèques Arduino.
4. Vérifier les broches et le câblage du montage avant de téléverser le programme.

Les bibliothèques tierces ne sont pas incluses dans le dépôt ; les installer depuis le gestionnaire de bibliothèques Arduino.

## Configuration Wi-Fi et Supabase

Les versions 2 et 3 utilisent des valeurs de remplacement dans leur configuration : `YOUR_WIFI_SSID`, `YOUR_WIFI_PASSWORD`, `https://YOUR_PROJECT.supabase.co` et `YOUR_SUPABASE_ANON_KEY`. Les remplacer localement avant utilisation. Ne jamais publier de mot de passe Wi-Fi, de clé API privée ou de clé Supabase `service_role`.
