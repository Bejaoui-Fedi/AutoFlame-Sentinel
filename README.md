# AutoFlame Sentinel — Système d’alarme incendie

**AutoFlame Sentinel** est un projet de fin d’études consacré à la conception et à la simulation d’un système d’alarme incendie à base d’ESP32. Le dépôt présente le projet, son montage et sa simulation interactive Wokwi.

## Simulation Wokwi

[Ouvrir le projet « Système Alarme Incendie » dans Wokwi](https://wokwi.com/projects/422714056118944769)

## Montage

Le montage illustré associe une carte ESP32 à un capteur de flamme, un capteur DHT22, un écran LCD, un buzzer, des voyants LED, un relais et un potentiomètre.

![Montage du système d’alarme incendie AutoFlame Sentinel](images/montage-alarme-incendie.png)

## Tests des matériels

Les sketches de test individuels des composants et les instructions de configuration sont disponibles dans le dossier [Tests pour les matériels](Testes%20pour%20les%20mat%C3%A9riels/README.md).

## Codes C++ du projet final

Les différentes versions des sketches Arduino/C++ sont regroupées dans le dossier [Les codes C++](Les%20codes%20C%2B%2B/README.md).

## Modèle IA

Le modèle TensorFlow Lite et ses métadonnées sont disponibles dans le dossier [Modèle IA](mod%C3%A9le%20IA/README.md).

## Données Supabase

Une copie expurgée des fichiers de stockage est disponible dans [`kxtyvffgdgamkugzraaz.storage.sanitized.zip`](kxtyvffgdgamkugzraaz.storage.sanitized.zip). La photo personnelle de l’archive originale en a été retirée.

L’export SQL [schéma-seulement](db_cluster-29-07-2025-schema-only.sql.gz) contient les définitions de la base sans les enregistrements ni les valeurs de séquence. Les rôles et privilèges ont été retirés, les URL et jetons sensibles expurgés, et le déclencheur webhook propre au projet omis. Ce fichier n’est pas une sauvegarde complète restaurable : les rôles, privilèges et paramètres du webhook devront être recréés séparément. La sauvegarde brute n’est pas publiée.
