#define LED_PIN 2  // GPIO2 est souvent utilisé pour les tests de LED

void setup() {
  pinMode(LED_PIN, OUTPUT);  // Déclare GPIO2 comme sortie
}

void loop() {
  digitalWrite(LED_PIN, HIGH);  // Allume la LED
  delay(1000);                  // Attends 1 seconde
  digitalWrite(LED_PIN, LOW);   // Éteint la LED
  delay(1000);                  // Attends 1 seconde
}
