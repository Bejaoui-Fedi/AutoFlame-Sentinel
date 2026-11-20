#define MOTOR_CONTROL_PIN 23 // Broche connectée à la base via 1kΩ

void setup() {
  pinMode(MOTOR_CONTROL_PIN, OUTPUT);
  digitalWrite(MOTOR_CONTROL_PIN, HIGH); // Active le transistor dès le début
  Serial.begin(115200);
  Serial.println("✅ Moteur activé en continu");
}

void loop() {
  // Ne rien faire ici pour garder le moteur ON
}
