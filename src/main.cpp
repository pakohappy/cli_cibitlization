#include <Arduino.h>
#include <WiFi.h>          // Librería WiFi estándar
//#include <EEPROM.h>

void printEncryptionType(const int thisType) {
    // read the encryption type and print out the name:
    switch (thisType) {
        case ENC_TYPE_WEP:
            Serial.println("WEP");
            break;
        case ENC_TYPE_TKIP:
            Serial.println("WPA");
            break;
        case ENC_TYPE_CCMP:
            Serial.println("WPA2");
            break;
        case ENC_TYPE_NONE:
            Serial.println("None");
            break;
        case ENC_TYPE_AUTO:
            Serial.println("Auto");
            break;
        default: ;
    }
}

void listNetworks() {
    // Buscando redes Wifi.
    Serial.println("** Scan Networks **");
    const char numSsid = WiFi.scanNetworks();
    if (numSsid == -1) {
        Serial.println("Couldn't get a wifi connection");
    }

    // Imprimir las redes al alcance.
    Serial.print("number of available networks:");
    Serial.println(numSsid);

    // print the network number and name for each network found:
    for (int thisNet = 0; thisNet < numSsid; thisNet++) {
        Serial.print(thisNet);
        Serial.print(") ");
        Serial.print(WiFi.SSID(thisNet));
        Serial.print("\tSignal: ");
        Serial.print(WiFi.RSSI(thisNet));
        Serial.print(" dBm");
        Serial.print("\tEncryption: ");
        printEncryptionType(WiFi.encryptionType(thisNet));
    }
}

/////////////////////
///     SETUP     ///
/////////////////////
void setup() {
    // Inicializar serial y esperar a puerto abierto.
    Serial.begin(9600);

    // Verificar la presencia de la interfaz wifi.
    if (WiFi.status() == WL_NO_SHIELD || WiFi.status() == WL_NO_MODULE) {
        Serial.println("La interfaz Wifi no está disponible.");
    }

    // Comprobar versión del firmaware.
    if (const String fv = WiFi.firmwareVersion(); fv != "1.1.0") {
        Serial.println("Porfavor actualice el firmware de la interfaz Wifi.");
    }
}

/////////////////////
///     LOOP      ///
/////////////////////
void loop() {
    while (WiFi.status() != WL_CONNECTED) {
        // Buscar las redes disponibles.
        Serial.println("Buscando redes disponibles...");
        listNetworks();

        // Elegir red..
        Serial.println("¿A qué red quieres conectarte? 0, 1, 2...");
        String red_wifi = Serial.readStringUntil('\n');
        // Convertimos red_wifi -> int.
        const int int_red_wifi = red_wifi.toInt();

        Serial.println("Red elegida: " + String(WiFi.SSID(int_red_wifi)));


        // Introducir contraseña.
        Serial.println("Introduce la contraseña:");
        String pass_wifi = Serial.readStringUntil('\n');

        // Conectarse a la res.
        WiFi.begin(WiFi.SSID(int_red_wifi), pass_wifi.c_str());
        // Esperar 10 segundos para conectar.
        delay(10000);
    }
    // Si se conecta, imprimir la red.
    Serial.println("Conectado a la red: " + String(WiFi.SSID()));

    delay(10000);
}