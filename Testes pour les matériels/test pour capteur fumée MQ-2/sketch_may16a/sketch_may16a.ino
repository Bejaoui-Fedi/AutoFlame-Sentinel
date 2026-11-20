#define MQ2_D0_PIN 3

void setup() {
  pinMode(MQ2_D0_PIN, INPUT);
  Serial.begin(115200);
}

void loop() {
  int etat = digitalRead(MQ2_D0_PIN);

  if (etat == LOW) {
    Serial.println("Fumee detectee");
  } else {
    Serial.println("Fumee non detectee");
  }

  delay(1000);
}
