#define BUZZER_PIN 15  // remplacez par la broche que vous utilisez

void setup() {
  // Rien à faire ici
}

void loop() {
  tone(BUZZER_PIN, 1000);  // Génère un son de 1000 Hz
  delay(1000);             // Attendre 1 seconde
  noTone(BUZZER_PIN);      // Arrête le son
  delay(1000);             // Attendre 1 seconde
}
