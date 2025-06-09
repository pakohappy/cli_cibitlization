#include <Arduino.h>
#include <WiFiManager.h> // Incluimos la librería WiFiManager
#include <EEPROM.h>      // Para guardar datos en la memoria

WiFiManager wifiManager;

void setup() {
    Serial.begin(115200);

    // Configuramos el WiFiManager
    wifiManager.autoConnect("ConfiguraWiFi"); // Nombre del punto de acceso

    // Si llegamos aquí es porque ya estamos conectados
    Serial.println("¡Conectado al WiFi!");
    Serial.println("IP: " + WiFi.localIP().toString());
}

void loop() {
    // Si presionas el botón de reset (pin definido)
    if (/* condición para resetear */) {
        Serial.println("Reseteando configuración WiFi...");
        wifiManager.resetSettings();
        ESP.restart(); // Reinicia el ESP
    }
}