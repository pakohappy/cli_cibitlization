#include <Arduino.h>
#include <WiFi.h>          // Librería WiFi estándar
#include <EEPROM.h>

// Direcciones EEPROM.
#define EEPROM_WIFI_SSID_ADDR 0
#define EEPROM_WIFI_PASS_ADDR 32
#define EEPROM_CREDENTIALS_FLAG_ADDR 100

bool hayCredencialesGuardadas() {
    return EEPROM.read(EEPROM_CREDENTIALS_FLAG_ADDR) == 1;
}


void guardarCredenciales(const String& ssid, const String& password) {
    // Limpiar las áreas de memoria
    for (int i = 0; i < 32; i++) {
        EEPROM.write(EEPROM_WIFI_SSID_ADDR + i, 0);
        EEPROM.write(EEPROM_WIFI_PASS_ADDR + i, 0);
    }

    // Guardar SSID
    for (size_t i = 0; i < ssid.length(); i++) {
        EEPROM.write(EEPROM_WIFI_SSID_ADDR + i, ssid[i]);
    }

    // Guardar Password
    for (size_t i = 0; i < password.length(); i++) {
        EEPROM.write(EEPROM_WIFI_PASS_ADDR + i, password[i]);
    }

    // Marcar que hay credenciales guardadas
    EEPROM.write(EEPROM_CREDENTIALS_FLAG_ADDR, 1);
}

void leerCredenciales(String& ssid, String& password) {
    ssid = "";
    password = "";

    // Leer SSID
    for (int i = 0; i < 32; i++) {
        const char c = EEPROM.read(EEPROM_WIFI_SSID_ADDR+ i);
        if (c == 0) break;
        ssid += c;
    }

    // Leer Password
    for (int i = 0; i < 32; i++) {
        char c = EEPROM.read(EEPROM_WIFI_PASS_ADDR + i);
        if (c == 0) break;
        password += c;
    }
}


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

void wifi_connect() {
    if (hayCredencialesGuardadas()) {
        String ssid, password;
        leerCredenciales(ssid, password);

        Serial.println("Intentando conectar con credenciales guardadas...");
        Serial.println("SSID: " + ssid);

        WiFi.begin(ssid.c_str(), password.c_str());

        // Esperar 10 segundos para conectar
        int intentos = 0;
        while (WiFi.status() != WL_CONNECTED && intentos < 20) {
            delay(500);
            Serial.print(".");
            intentos++;
        }
        Serial.println();

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Conectado a la red: " + String(WiFi.SSID()));
            return;
        }
        Serial.println("No se pudo conectar con las credenciales guardadas");
    }

    // Si no hay credenciales o la conexión falló, proceder con el método manual
    while (WiFi.status() != WL_CONNECTED) {
        // Buscar las redes disponibles.
        Serial.println("Buscando redes disponibles...");
        listNetworks();

        // Elegir red
        Serial.println("¿A qué red quieres conectarte? 0, 1, 2...");

        // Esperar hasta que haya datos disponibles
        while (!Serial.available()) {
            delay(100);  // Pequeña pausa para no saturar la CPU.
        }

        String red_wifi = "";
        // Leer mientras haya datos y hasta encontrar un salto de línea
        while (Serial.available()) {
            char c = Serial.read();
            if (c == '\n') break;
            red_wifi += c;
            delay(2);  // Pequeña pausa para asegurar lectura correcta
        }

        int red_numero = red_wifi.toInt();
        Serial.println("Red elegida: " + String(WiFi.SSID(red_numero)));

        // Introducir contraseña
        Serial.println("Introduce la contraseña:");

        // Limpiar buffer antes de leer
        while(Serial.available()) {
            Serial.read();
        }

        // Esperar nueva entrada
        while (!Serial.available()) {
            delay(100);
        }

        String pass_wifi = "";
        while (Serial.available()) {
            const char c = Serial.read();
            if (c == '\n') break;
            pass_wifi += c;
            delay(2);
        }

        Serial.println("Intentando conectar...");

        // Connect to network.
        WiFi.begin(WiFi.SSID(red_numero), pass_wifi.c_str());

        if (WiFi.status() == WL_CONNECTED) {
            guardarCredenciales(WiFi.SSID(red_numero), pass_wifi);
            Serial.println("Credenciales guardadas en EEPROM");
        }
        // Esperar 10 segundos para conectar
        delay(10000);
    }

    // Si se conecta, imprimir la red
    Serial.println("Conectado a la red: " + String(WiFi.SSID()));
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
    wifi_connect();

    delay(10000);
}
