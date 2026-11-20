 #include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "YOUR_WIFI_SSID";        // Changez avec votre SSID
const char* password = "YOUR_WIFI_PASSWORD";      // Changez avec votre mot de passe


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connexion WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connecté !");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = "https://YOUR_PROJECT.supabase.co";
    String apiKey = "YOUR_SUPABASE_ANON_KEY";

    String jsonPayload = "{";
    jsonPayload += "\"temperature\":25.5,";
    jsonPayload += "\"humidity\":60.2,";
    jsonPayload += "\"gas\":123.4,";
    jsonPayload += "\"flame\":false,";
    jsonPayload += "\"smoke\":true";
    jsonPayload += "}";

    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("apikey", apiKey);
    http.addHeader("Authorization", "Bearer " + apiKey);

    int httpResponseCode = http.POST(jsonPayload);
    Serial.print("Code réponse Supabase : ");
    Serial.println(httpResponseCode);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Réponse Supabase : " + response);
    } else {
      Serial.println("Erreur d'envoi !");
    }

    http.end();
  }

  delay(10000); // Attendre 10 secondes avant de renvoyer
}
