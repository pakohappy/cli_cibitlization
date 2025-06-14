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
    const int numSsid = WiFi.scanNetworks();
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

void loop() {
    // Buscar las redes disponibles.
    Serial.println("Buscando redes disponibles...");
    listNetworks();
    delay(10000);
}