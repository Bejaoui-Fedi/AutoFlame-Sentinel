#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "YOUR_WIFI_SSID";        // Changez avec votre SSID
const char* password = "YOUR_WIFI_PASSWORD";      // Changez avec votre mot de passe

WebServer server(80); // Port 80 (HTTP)

void handleRoot() {
  server.send(200, "text/html", "<h1>ESP32 connecté avec succès !</h1><p>Bienvenue dans votre serveur web embarqué.</p>");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.println("Connexion au Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ Connecté au Wi-Fi !");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("🌐 Serveur web démarré !");
}

void loop() {
  server.handleClient();
}
