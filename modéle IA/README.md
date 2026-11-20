# Modèle IA — détection du risque incendie

Ce dossier contient le modèle TensorFlow Lite `fire_risk_model.tflite` et ses métadonnées.

Les métadonnées décrivent les plages d’entrée utilisées pour le gaz (0–150), la température (15–45 °C) et l’humidité (30–95 %), ainsi que les classes de sortie : `low`, `medium` et `high`.

Fichiers :

- `fire_risk_model.tflite` — modèle TensorFlow Lite.
- `model_metadata.json` — plages de mise à l’échelle et libellés de sortie.
