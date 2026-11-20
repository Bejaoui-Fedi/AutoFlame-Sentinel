#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// Initialisation LCD I2C
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Définition des broches
#define DHTPIN 4
#define DHTTYPE DHT22
#define MQ2_PIN 34         // Gaz - MQ-2 (analogique)
#define MQ135_PIN 35       // Fumée - MQ-135 (analogique)
#define FLAME_PIN 32       // Capteur de flamme (analogique)
#define BUZZER_PIN 19
#define GREEN_LED_PIN 2
#define RED_LED_PIN 15
#define BLUE_LED_PIN 18    // LED bleue pour extincteur (flamme détectée)

// Seuils CORRIGÉS
const float tempThreshold = 40.0;
const float humidityThreshold = 20.0;
const float gasPPMThreshold = 200.0;  // PPM - seuil réaliste !
const int smokeAnalogThreshold = 1500;
const int flameAnalogThreshold = 2000;  // Seuil ajusté - plus sensible

// Constantes pour conversion PPM - CALIBRATION RAPIDE !
const float RL_VALUE = 5.0;           // Résistance de charge en kΩ
const float RO_CLEAN_AIR_FACTOR = 9.83;  // Facteur RO dans l'air propre
const float CALIBRATION_SAMPLE_TIMES = 10;    // RÉDUIT de 50 à 10 !
const float CALIBRATION_SAMPLE_INTERVAL = 50; // RÉDUIT de 500ms à 50ms !
const float READ_SAMPLE_INTERVAL = 20;         // RÉDUIT de 50ms à 20ms !
const float READ_SAMPLE_TIMES = 3;             // RÉDUIT de 5 à 3 !

// Variables de calibration
float Ro = 10;  // Valeur de résistance dans l'air propre

// DHT init
DHT dht(DHTPIN, DHTTYPE);

// Timing
unsigned long lastSensorRead = 0;
const unsigned long sensorInterval = 1000;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  // Message de bienvenue
  lcd.setCursor(6, 0);
  lcd.print("BIENVENUE");
  lcd.setCursor(0, 1);
  lcd.print("SYSTEME DE DETECTION");
  lcd.setCursor(6, 2);
  lcd.print("D'INCENDIE");
  delay(3000);

  // Calibration ULTRA-RAPIDE du capteur MQ-2 (silencieuse)
  Ro = MQCalibration(MQ2_PIN);

  // Configuration des broches
  pinMode(MQ2_PIN, INPUT);
  pinMode(MQ135_PIN, INPUT);
  pinMode(FLAME_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);  // Configuration LED bleue extincteur

  dht.begin();

  Serial.println("Système prêt!");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastSensorRead >= sensorInterval) {
    lastSensorRead = currentMillis;

    // Lecture des capteurs
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    // Conversion en PPM pour le gaz (MQ-2)
    float gasPPM = MQGetGasPercentage(MQRead(MQ2_PIN) / Ro);

    // Lecture des valeurs analogiques pour debug
    int smokeValue = analogRead(MQ135_PIN);
    int flameValue = analogRead(FLAME_PIN);

    bool smokeDetected = smokeValue > smokeAnalogThreshold;
    bool flameDetected = flameValue < flameAnalogThreshold;

    // Vérification DHT
    if (isnan(temp) || isnan(hum)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Erreur capteur DHT");
      return;
    }

    // Détection alarme avec seuil PPM correct
    bool alert = false;
    if (flameDetected || smokeDetected || gasPPM > gasPPMThreshold ||
        temp > tempThreshold || hum < humidityThreshold) {
      alert = true;
    }

    // Gestion des LEDs et buzzer
    if (alert) {
      digitalWrite(RED_LED_PIN, HIGH);
      digitalWrite(GREEN_LED_PIN, LOW);
      tone(BUZZER_PIN, 1000);  // BIP simple et normal
    } else {
      digitalWrite(RED_LED_PIN, LOW);
      digitalWrite(GREEN_LED_PIN, HIGH);
      noTone(BUZZER_PIN);
    }

    // Gestion LED bleue extincteur - s'allume UNIQUEMENT si flamme détectée
    if (flameDetected) {
      digitalWrite(BLUE_LED_PIN, HIGH);  // Extincteur activé !
    } else {
      digitalWrite(BLUE_LED_PIN, LOW);   // Pas de flamme = pas d'extincteur
    }

    // Affichage LCD avec PPM
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temp, 1);
    lcd.print("C H:");
    lcd.print(hum, 0);
    lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("Gaz:");
    lcd.print(gasPPM, 0);
    lcd.print(" PPM");

    lcd.setCursor(0, 2);
    lcd.print("Fumee:");
    lcd.print(smokeDetected ? "OUI " : "NON");

    lcd.setCursor(0, 3);
    lcd.print("Flamme:");
    lcd.print(flameDetected ? "OUI " : "NON");
    if (flameDetected) {
      lcd.print(" EXT!");  // Indication extincteur sur LCD
    }

    // Debug série avec valeurs brutes
    Serial.print("Gaz: ");
    Serial.print(gasPPM);
    Serial.print(" PPM - Fumée: ");
    Serial.print(smokeValue);
    Serial.print(" - Flamme: ");
    Serial.print(flameValue);
    Serial.print(" (seuil: ");
    Serial.print(flameAnalogThreshold);
    Serial.print(") - Détectée: ");
    Serial.print(flameDetected ? "OUI" : "NON");
    if (flameDetected) {
      Serial.print(" - EXTINCTEUR ACTIVE!");
    }
    Serial.println();
  }
}

// Fonction de calibration du MQ-2 (ULTRA-RAPIDE)
float MQCalibration(int mq_pin) {
  int i;
  float val = 0;

  for (i = 0; i < CALIBRATION_SAMPLE_TIMES; i++) {
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val / CALIBRATION_SAMPLE_TIMES;
  val = val / RO_CLEAN_AIR_FACTOR;

  return val;
}

// Calcul de la résistance du capteur
float MQResistanceCalculation(int raw_adc) {
  return (((float)RL_VALUE * (4095 - raw_adc) / raw_adc));
}

// Lecture moyennée du capteur (RAPIDE)
float MQRead(int mq_pin) {
  int i;
  float rs = 0;

  for (i = 0; i < READ_SAMPLE_TIMES; i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }

  rs = rs / READ_SAMPLE_TIMES;
  return rs;
}

// Conversion en PPM (approximative pour gaz combustibles)
float MQGetGasPercentage(float rs_ro_ratio) {
  // Courbe approximative pour gaz combustibles (LPG/Propane)
  // Formule: PPM = a * (rs_ro_ratio)^b
  // Pour MQ-2: a ≈ 574, b ≈ -2.222
  return (574.0 * pow(rs_ro_ratio, -2.222));
}
