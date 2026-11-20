 #include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Configuration WiFi
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Configuration Supabase
const String supabaseUrl = "https://YOUR_PROJECT.supabase.co";
const String apiKey = "YOUR_SUPABASE_ANON_KEY";

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
#define BLUE_LED_PIN 18    // LED bleue pour extincteur
#define MOTOR_PIN 23       // Moteur DC (ventilateur)

// Seuils
const float tempThreshold = 40.0;
const float humidityThreshold = 20.0;
const float gasPPMThreshold = 200.0;
const int smokeAnalogThreshold = 2000;
const int flameAnalogThreshold = 2000;

// Constantes pour conversion PPM
const float RL_VALUE = 5.0;
const float RO_CLEAN_AIR_FACTOR = 9.83;
const float CALIBRATION_SAMPLE_TIMES = 10;
const float CALIBRATION_SAMPLE_INTERVAL = 50;
const float READ_SAMPLE_INTERVAL = 20;
const float READ_SAMPLE_TIMES = 3;

// Variables
float Ro = 10;
DHT dht(DHTPIN, DHTTYPE);

// Timing
unsigned long lastSensorRead = 0;
unsigned long lastSupabaseUpdate = 0;
unsigned long lastBuzzerToggle = 0;
unsigned long lastLedToggle = 0;
unsigned long lastSupabaseCheck = 0;  // Pour vérifier les commandes depuis l'app
const unsigned long sensorInterval = 1000;
const unsigned long supabaseInterval = 10000;
const unsigned long buzzerInterval = 500;
const unsigned long ledInterval = 250;
const unsigned long supabaseCheckInterval = 2000;  // Vérifier toutes les 2 secondes

// Variables d'état
bool previousFlameState = false;
bool previousVentilatorState = false;
bool buzzerState = false;
bool ledState = false;
bool alertActive = false;
bool manualExtincteurActive = false;  // État de l'extincteur contrôlé depuis l'app
bool manualVentilateurActive = false;  // État du ventilateur contrôlé depuis l'app

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

  // Connexion WiFi
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connexion WiFi...");
  WiFi.begin(ssid, password);
  Serial.print("Connexion WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.setCursor(0, 1);
    lcd.print("En cours...");
  }

  Serial.println("\nWiFi connecté !");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi: CONNECTE");
  lcd.setCursor(0, 1);
  lcd.print("IP: ");
  lcd.print(WiFi.localIP());
  delay(2000);

  // Calibration capteur MQ-2
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibration...");
  Ro = MQCalibration(MQ2_PIN);

  // Configuration des broches
  pinMode(MQ2_PIN, INPUT);
  pinMode(MQ135_PIN, INPUT);
  pinMode(FLAME_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);

  dht.begin();

  Serial.println("Système prêt!");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SYSTEME PRET!");
  delay(1000);
}

void loop() {
  unsigned long currentMillis = millis();

  // Gestion des alertes
  handleAlerts(currentMillis);

  // Vérifier les commandes depuis l'application
  if (currentMillis - lastSupabaseCheck >= supabaseCheckInterval) {
    lastSupabaseCheck = currentMillis;
    checkSupabaseCommands();
  }

  if (currentMillis - lastSensorRead >= sensorInterval) {
    lastSensorRead = currentMillis;

    // Lecture des capteurs
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    float gasPPM = MQGetGasPercentage(MQRead(MQ2_PIN) / Ro);

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

    // Gestion du moteur DC (ventilateur) - automatique OU manuel
    bool ventilateurShouldBeOn = (temp > tempThreshold) || manualVentilateurActive;

    if (ventilateurShouldBeOn) {
      digitalWrite(MOTOR_PIN, HIGH);  // Activer le ventilateur
    } else {
      digitalWrite(MOTOR_PIN, LOW);   // Désactiver le ventilateur
    }

    // Si l'état du ventilateur a changé, mettre à jour la base
    if (ventilateurShouldBeOn != previousVentilatorState) {
      updateVentilateurState(ventilateurShouldBeOn);
      previousVentilatorState = ventilateurShouldBeOn;
    }

    // Détection alarme - SANS manualExtincteurActive (juste pour LED bleue)
    alertActive = false;
    if (flameDetected || smokeDetected || gasPPM > gasPPMThreshold ||
        temp > tempThreshold || hum < humidityThreshold) {
      alertActive = true;
    }

    // Gestion LED verte
    if (!alertActive) {
      digitalWrite(GREEN_LED_PIN, HIGH);
    } else {
      digitalWrite(GREEN_LED_PIN, LOW);
    }

    // Si l'état de la flamme a changé, mettre à jour l'extincteur
    if (flameDetected != previousFlameState) {
      updateExtincteurState(flameDetected);
      previousFlameState = flameDetected;
    }

    // Affichage LCD harmonisé - 4 lignes distinctes
    lcd.clear();

    // Ligne 1 : Température et Humidité
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temp, 1);
    lcd.print("C H:");
    lcd.print(hum, 0);
    lcd.print("%");

    // Ligne 2 : Gaz et Fumée
    lcd.setCursor(0, 1);
    lcd.print("Gaz:");
    lcd.print(gasPPM, 0);
    lcd.print(" Fumee:");
    lcd.print(smokeDetected ? "OUI" : "NON");

    // Ligne 3 : État de l'extincteur
    lcd.setCursor(0, 2);
    if (manualExtincteurActive) {
      lcd.print("EXTINCTEUR: MANUEL");
    } else {
      lcd.print("Flamme:");
      lcd.print(flameDetected ? "OUI" : "NON");
      if (flameDetected) {
        lcd.print(" EXT!");
      }
    }

    // Ligne 4 : État du ventilateur
    lcd.setCursor(0, 3);
    if (manualVentilateurActive) {
      lcd.print("VENTILATEUR: MANUEL");
    } else {
      lcd.print("Temp:");
      lcd.print(temp > tempThreshold ? "ELEV" : "OK");
      if (ventilateurShouldBeOn) {
        lcd.print(" VENT!");
      }
    }

    // Debug série
    Serial.print("T:");
    Serial.print(temp);
    Serial.print(" H:");
    Serial.print(hum);
    Serial.print(" Gaz:");
    Serial.print(gasPPM);
    Serial.print(" Fumée:");
    Serial.print(smokeValue);
    Serial.print(" Flamme:");
    Serial.print(flameValue);
    Serial.print(" Détectée:");
    Serial.print(flameDetected ? "OUI" : "NON");
    Serial.print(" Manuel:");
    Serial.print(manualExtincteurActive ? "OUI" : "NON");
    Serial.print(" Ventilateur:");
    Serial.print(ventilateurShouldBeOn ? "ON" : "OFF");
    Serial.print(" (Manuel:");
    Serial.print(manualVentilateurActive ? "OUI" : "NON");
    Serial.println(")");

    // Envoi périodique vers Supabase
    if (currentMillis - lastSupabaseUpdate >= supabaseInterval) {
      lastSupabaseUpdate = currentMillis;
      sendSensorDataToSupabase(temp, hum, gasPPM, flameDetected, smokeDetected);
    }
  }
}

void handleAlerts(unsigned long currentMillis) {
  // LED bleue : toujours gérée indépendamment (flamme OU contrôle manuel)
  digitalWrite(BLUE_LED_PIN, (previousFlameState || manualExtincteurActive) ? HIGH : LOW);

  if (alertActive) {
    // Gestion buzzer clignotant
    if (currentMillis - lastBuzzerToggle >= buzzerInterval) {
      lastBuzzerToggle = currentMillis;
      buzzerState = !buzzerState;

      if (buzzerState) {
        tone(BUZZER_PIN, 1000);
      } else {
        noTone(BUZZER_PIN);
      }
    }

    // Gestion LED rouge clignotante
    if (currentMillis - lastLedToggle >= ledInterval) {
      lastLedToggle = currentMillis;
      ledState = !ledState;
      digitalWrite(RED_LED_PIN, ledState ? HIGH : LOW);
    }

  } else {
    // Pas d'alerte - éteindre buzzer et LED rouge seulement
    noTone(BUZZER_PIN);
    digitalWrite(RED_LED_PIN, LOW);
    buzzerState = false;
    ledState = false;
  }
}

void sendSensorDataToSupabase(float temp, float hum, float gas, bool flame, bool smoke) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi non connecté - envoi annulé");
    return;
  }

  HTTPClient http;
  String url = supabaseUrl + "capteurs";

  String jsonPayload = "{";
  jsonPayload += "\"temperature\":" + String(temp, 2) + ",";
  jsonPayload += "\"humidity\":" + String(hum, 2) + ",";
  jsonPayload += "\"gas\":" + String(gas, 2) + ",";
  jsonPayload += "\"flame\":";
  jsonPayload += flame ? "true" : "false";
  jsonPayload += ",";
  jsonPayload += "\"smoke\":";
  jsonPayload += smoke ? "true" : "false";
  jsonPayload += "}";

  Serial.println("=== ENVOI CAPTEURS ===");
  Serial.println("URL: " + url);
  Serial.println("JSON: " + jsonPayload);

  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("apikey", apiKey);
  http.addHeader("Authorization", "Bearer " + apiKey);
  http.setTimeout(5000);

  int httpResponseCode = http.POST(jsonPayload);

  Serial.print("Code réponse: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Réponse complète: " + response);
    if (httpResponseCode == 201) {
      Serial.println("✓ Données insérées avec succès!");
    }
  } else {
    Serial.println("✗ Erreur HTTP - Vérifiez la table et RLS!");
  }

  http.end();
}

// Vérifier les commandes depuis l'application mobile
void checkSupabaseCommands() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  // Vérifier extincteur_state
  HTTPClient http;
  String url = supabaseUrl + "extincteur_state?device_id=eq.ESP32_1&order=last_updated.desc&limit=1";

  http.begin(url);
  http.addHeader("apikey", apiKey);
  http.addHeader("Authorization", "Bearer " + apiKey);
  http.setTimeout(3000);

  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    String response = http.getString();

    if (response.length() > 10) {
      bool commandActive = false;
      if (response.indexOf("\"is_active\":true") != -1) {
        commandActive = true;
      } else if (response.indexOf("\"is_active\":false") != -1) {
        commandActive = false;
      }

      if (commandActive != manualExtincteurActive) {
        manualExtincteurActive = commandActive;

        Serial.println("=== COMMANDE EXTINCTEUR REÇUE ===");
        Serial.print("Extincteur manuel: ");
        Serial.println(manualExtincteurActive ? "ACTIVÉ" : "DÉSACTIVÉ");
      }
    }
  }
  http.end();

  // Vérifier ventilateur_state
  HTTPClient http2;
  String url2 = supabaseUrl + "ventilateur_state?device_id=eq.ESP32_1&order=last_updated.desc&limit=1";

  http2.begin(url2);
  http2.addHeader("apikey", apiKey);
  http2.addHeader("Authorization", "Bearer " + apiKey);
  http2.setTimeout(3000);

  int httpResponseCode2 = http2.GET();

  if (httpResponseCode2 == 200) {
    String response2 = http2.getString();

    if (response2.length() > 10) {
      bool commandActive2 = false;
      if (response2.indexOf("\"is_active\":true") != -1) {
        commandActive2 = true;
      } else if (response2.indexOf("\"is_active\":false") != -1) {
        commandActive2 = false;
      }

      if (commandActive2 != manualVentilateurActive) {
        manualVentilateurActive = commandActive2;

        Serial.println("=== COMMANDE VENTILATEUR REÇUE ===");
        Serial.print("Ventilateur manuel: ");
        Serial.println(manualVentilateurActive ? "ACTIVÉ" : "DÉSACTIVÉ");
      }
    }
  }
  http2.end();
}

void updateExtincteurState(bool isActive) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi non connecté - mise à jour extincteur annulée");
    return;
  }

  HTTPClient http;
  String url = supabaseUrl + "extincteur_state";

  String jsonPayload = "{";
  jsonPayload += "\"device_id\":\"ESP32_1\",";
  jsonPayload += "\"is_active\":";
  jsonPayload += isActive ? "true" : "false";
  jsonPayload += "}";

  Serial.println("=== NOUVEL ÉTAT EXTINCTEUR ===");
  Serial.println("URL: " + url);
  Serial.println("Payload: " + jsonPayload);

  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("apikey", apiKey);
  http.addHeader("Authorization", "Bearer " + apiKey);
  http.setTimeout(5000);

  int httpResponseCode = http.POST(jsonPayload);

  Serial.print("Extincteur POST - Code: ");
  Serial.print(httpResponseCode);

  if (httpResponseCode == 201) {
    Serial.println(" - Nouvel enregistrement créé!");
    String response = http.getString();
    Serial.println("Réponse: " + response);
  } else if (httpResponseCode > 0) {
    Serial.println(" - Erreur!");
    String response = http.getString();
    Serial.println("Erreur: " + response);
  } else {
    Serial.println(" - Erreur de connexion!");
  }

  http.end();

  Serial.print("Nouvel état extincteur: ");
  Serial.println(isActive ? "ACTIF" : "INACTIF");
}

void updateVentilateurState(bool isActive) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi non connecté - mise à jour ventilateur annulée");
    return;
  }

  HTTPClient http;
  String url = supabaseUrl + "ventilateur_state";

  String jsonPayload = "{";
  jsonPayload += "\"device_id\":\"ESP32_1\",";
  jsonPayload += "\"is_active\":";
  jsonPayload += isActive ? "true" : "false";
  jsonPayload += "}";

  Serial.println("=== NOUVEL ÉTAT VENTILATEUR ===");
  Serial.println("URL: " + url);
  Serial.println("Payload: " + jsonPayload);

  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("apikey", apiKey);
  http.addHeader("Authorization", "Bearer " + apiKey);
  http.setTimeout(5000);

  int httpResponseCode = http.POST(jsonPayload);

  Serial.print("Ventilateur POST - Code: ");
  Serial.print(httpResponseCode);

  if (httpResponseCode == 201) {
    Serial.println(" - Nouvel enregistrement créé!");
    String response = http.getString();
    Serial.println("Réponse: " + response);
  } else if (httpResponseCode > 0) {
    Serial.println(" - Erreur!");
    String response = http.getString();
    Serial.println("Erreur: " + response);
  } else {
    Serial.println(" - Erreur de connexion!");
  }

  http.end();

  Serial.print("Nouvel état ventilateur: ");
  Serial.println(isActive ? "ACTIF" : "INACTIF");
}

// Fonctions MQ-2
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

float MQResistanceCalculation(int raw_adc) {
  return (((float)RL_VALUE * (4095 - raw_adc) / raw_adc));
}

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

float MQGetGasPercentage(float rs_ro_ratio) {
  return (574.0 * pow(rs_ro_ratio, -2.222));
}
