#include <Arduino.h>

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <LittleFS.h>
  #include <ESP8266HTTPClient.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include "SPIFFS.h"
  #include <HTTPClient.h>
#endif

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); // Mode station seulement
  
  // Connexion à un réseau Wi-Fi ouvert
  WiFi.begin("NomDuReseauOuvert");  // Réseau ouvert, pas de mot de passe
  Serial.println("Connexion au réseau Wi-Fi ouvert...");
  
  // Attendre la connexion au réseau
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Tentative de connexion...");
  }
  
  Serial.println("Connecté au réseau ouvert");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());
  
  // Tentative d'accès à une URL pour vérifier la connexion complète
  HTTPClient http;
  http.begin("https://portal-selfcare.wifirst.net/");  // Remplacez par une URL test (peut être un site simple comme http://example.com)
  int httpCode = http.GET();

  if (httpCode > 0) {
    // Réponse HTTP reçue
    if (httpCode == HTTP_CODE_OK) {
      Serial.println("Connexion réussie, accès à internet disponible");
    } else {
      Serial.println("Redirigé vers une page captive, l'authentification est requise");
      // Ici, vous pouvez fournir des instructions à l'utilisateur pour qu'il s'authentifie manuellement
    }
  } else {
    Serial.println("Impossible d'accéder à la page de test");
  }
  
  http.end();

  // Démarrer un point d'accès si vous voulez permettre l'accès à une interface de configuration
  WiFi.softAP("MyESP_AccessPoint", "motDePasse");
  Serial.println("Point d'accès démarré");
  
  server.begin();
}

void loop() {
  // Boucle principale
}
