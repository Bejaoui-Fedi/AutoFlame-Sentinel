#define FLAME_PIN 3  // Broche DO du capteur branchée sur GPIO4

void setup() {
  Serial.begin(115200);
  pinMode(FLAME_PIN, INPUT);
  Serial.println("Détection de flamme (sortie numérique)");
}

void loop() {
  int flameDetected = digitalRead(FLAME_PIN);

  if (flameDetected == LOW) {  // LOW = flamme détectée (logique inverse typique)
    Serial.println("Flamme détectée !");
  } else {
    Serial.println("Pas de flamme");
  }

  delay(500);  // Délai pour éviter le spam du moniteur série
}
