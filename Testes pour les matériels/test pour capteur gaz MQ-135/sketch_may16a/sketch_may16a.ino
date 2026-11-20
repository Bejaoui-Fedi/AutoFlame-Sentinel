#define MQ135_PIN 34  // Broche analogique de l'ESP32
float R0 = 76.63;     // Valeur à calibrer selon ton capteur et l'air propre

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Capteur MQ-135 - Lecture en ppm");
}

void loop() {
  int adcValue = analogRead(MQ135_PIN);
  float voltage = adcValue * (3.3 / 4095.0);  // Tension en volts
  float RS = (3.3 - voltage) / voltage;       // Résistance RS du capteur

  float ratio = RS / R0;

  // Approximation pour CO2 en ppm (valeur indicative, non certifiée)
  float ppm = 116.6020682 * pow(ratio, -2.769034857);

  Serial.print("Valeur ADC : ");
  Serial.print(adcValue);
  Serial.print(" | PPM estimé : ");
  Serial.println(ppm, 2);

  delay(1000);
}
