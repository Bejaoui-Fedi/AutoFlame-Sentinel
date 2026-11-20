#include "DHT.h"

#define DHTPIN 4         // Le pin GPIO où le DATA du DHT22 est connecté
#define DHTTYPE DHT22    // Type du capteur

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  delay(2000); // Laisser le capteur se stabiliser
  Serial.println("Test du capteur DHT22 avec ESP32");
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Celsius

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Échec de lecture du capteur DHT22 !");
  } else {
    Serial.print("Température : ");
    Serial.print(temperature);
    Serial.print(" °C  |  Humidité : ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  delay(2000); // Lecture toutes les 2 secondes
}
