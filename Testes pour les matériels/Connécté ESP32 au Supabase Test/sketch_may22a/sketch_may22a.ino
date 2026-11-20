#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "YOUR_WIFI_SSID";        // Changez avec votre SSID
const char* password = "YOUR_WIFI_PASSWORD";      // Changez avec votre mot de passe

const char* SUPABASE_URL = "https://YOUR_PROJECT.supabase.co"; // <-- mets ton URL
const char* SUPABASE_API_KEY = "YOUR_SUPABASE_ANON_KEY"; // <-- ta clé anon

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connexion WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnecté au WiFi !");

  envoyerTest();
}

void loop() {
  // rien à faire ici pour ce test
}

void envoyerTest() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(SUPABASE_URL);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("apikey", SUPABASE_API_KEY);
    http.addHeader("Authorization", ("Bearer " + String(SUPABASE_API_KEY)).c_str());

    String json = "{\"temperature\": 22.5}";

    int code = http.POST(json);
    Serial.print("Code HTTP : ");
    Serial.println(code);

    if (code > 0) {
      String reponse = http.getString();
      Serial.println("Réponse Supabase : " + reponse);
    } else {
      Serial.println("Erreur d'envoi !");
    }

    http.end();
  }
}
