#include <Arduino.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <LittleFS.h>
  #include <ESP8266mDNS.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include "SPIFFS.h"
  #include <ESPmDNS.h>
#endif
#include <ESPConnect.h>
#include <PubSubClient.h> // Bibliothèque MQTT

AsyncWebServer server(80);
WiFiClient espClient;
PubSubClient mqttClient(espClient);

const char* mqtt_server = "34.155.149.235 ";
const char* mqtt_topic = "esp/chip_id"; // Sujet MQTT sur lequel vous voulez publier

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  Serial.println("\nDémarrage de l'ESP8266/ESP32");

#if defined(ESP8266)
  if (!LittleFS.begin()) {
    Serial.println("Erreur lors de l'initialisation de LittleFS");
    return;
  }
#elif defined(ESP32)
  if (!SPIFFS.begin()) {
    Serial.println("Erreur lors de l'initialisation de SPIFFS");
    return;
  }
#endif

  String customSSID = "Esp8266";
  String password = "password";
  ESPConnect.autoConnect(customSSID.c_str(), password.c_str());

  if (ESPConnect.begin(&server)) {
    Serial.println("Connecté au Wi-Fi");
    Serial.println("Adresse IP: " + WiFi.localIP().toString());

    if (MDNS.begin("esp8266")) {
      Serial.println("mDNS démarré avec succès.");
    } else {
      Serial.println("Erreur lors de l'initialisation de mDNS");
    }

    // Configuration du client MQTT
    mqttClient.setServer(mqtt_server, 1883);
  } else {
    Serial.println("Échec de la connexion au Wi-Fi");
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String chipID;
#if defined(ESP8266)
    chipID = String(ESP.getChipId());
#elif defined(ESP32)
    chipID = String((uint32_t)ESP.getEfuseMac(), HEX);
#endif
    String redirectURL = "/index.html?chipID=" + chipID;
    request->redirect(redirectURL);
  });

#if defined(ESP8266)
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });
#elif defined(ESP32)
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
#endif

  server.begin();
  Serial.println("Serveur démarré");
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connexion au serveur MQTT...");
    if (mqttClient.connect("ESP8266Client")) {
      Serial.println("Connecté au serveur MQTT");
    } else {
      Serial.print("Échec de la connexion, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Nouvelle tentative dans 5 secondes...");
      delay(5000);
    }
  }
}

void publishMessage() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.publish(mqtt_topic, "Bonjour du ESP8266/ESP32");
  Serial.println("Message MQTT envoyé");
}

void loop() {
  while(!MDNS.update());
  mqttClient.loop(); // Maintenir la connexion MQTT
  publishMessage();  // Envoyer le message, ici vous pouvez ajouter une condition ou un délai
  delay(10000);      // Délai de 10 secondes entre les messages pour éviter un envoi continu
}
