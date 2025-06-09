#include <Arduino.h>
#include <WiFi.h>          // Librería WiFi estándar
#include <EEPROM.h>

// Configuración WiFi
const char* ssid = "TuRedWiFi";         // Nombre de tu red WiFi
const char* password = "TuContraseña";   // Contraseña de tu red WiFi

// Variables para el estado del WiFi
unsigned long previousMillis = 0;
unsigned long interval = 30000;  // Intervalo para verificar la conexión (30 segundos)

void setupWiFi() {
    WiFi.mode(WIFI_STA);  // Modo estación
    WiFi.begin(ssid, password);

    Serial.print("Conectando a WiFi");
    int intentos = 0;

    while (WiFi.status() != WL_CONNECTED && intentos < 20) {  // Tiempo máximo de espera
        delay(500);
        Serial.print(".");
        intentos++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n¡Conectado!");
        Serial.print("Dirección IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("Fuerza de la señal (RSSI): ");
        Serial.println(WiFi.RSSI());
    } else {
        Serial.println("\nError al conectar");
    }
}

void checkWiFiConnection() {
    unsigned long currentMillis = millis();

    if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)) {
        Serial.println("Reconectando WiFi...");
        WiFi.disconnect();
        WiFi.begin(ssid, password);
        previousMillis = currentMillis;
    }
}

void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.println("\nIniciando conexión WiFi...");
    setupWiFi();
}

void loop() {
    checkWiFiConnection();  // Monitorea la conexión WiFi

    // Aquí puedes añadir tu código principal
    if (WiFi.status() == WL_CONNECTED) {
        // Realizar acciones que requieran conexión WiFi
    }

    delay(1000);
}
