#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "....";
const char *password = "....";

int relayPin = 2;
boolean lightOn = false;

ESP8266WebServer server(8081);

void updateRelayPin() {
  int pinValue = lightOn ? 1 : 0;
  digitalWrite(relayPin, pinValue);
}

void configureLightOn(boolean turnOn) {
  lightOn = turnOn;
  updateRelayPin();  
}

void handleLightOn() {
  configureLightOn(true);
  server.send(200, "application/json", "{\"lightOn\":true}");
}

void handleLightOff() {
  configureLightOn(false);
  server.send(200, "application/json", "{\"lightOn\":false}");
}

void handleLightStatus() {
  String resp = String(String("{\"lightOn\":") + String(lightOn ? "true" : "false") + String("}"));
  server.send(200, "application/json", resp);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup() {
  pinMode(relayPin, OUTPUT);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/light/status", handleLightStatus);
  server.on("/light/on", handleLightOn);
  server.on("/light/off", handleLightOff);
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

