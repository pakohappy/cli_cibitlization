#include <Arduino.h>
#include <WiFi.h>          // Librería WiFi estándar
#include <EEPROM.h>
#include "Arduino_LED_Matrix.h"
#include "libs/animations/logoscroll.h"

// Direcciones EEPROM.
#define EEPROM_WIFI_SSID_ADDR 0
#define EEPROM_WIFI_PASS_ADDR 32
#define EEPROM_CREDENTIALS_FLAG_ADDR 100

ArduinoLEDMatrix matrix;

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
    Serial.println("** Iniciando escaneo de redes **");

    WiFi.disconnect();  // Desconecta de cualquier red previa
    delay(1000);  // Espera 1 segundo

    Serial.println("Escaneando redes WiFi...");
    int numSsid = WiFi.scanNetworks();  // scan async = false, show_hidden = true

    // Espera más tiempo para el escaneo
    if (WiFi.status() == WL_SCAN_COMPLETED) {
        Serial.println("Escaneo completado");
    }

    Serial.println("");

    if (numSsid == -1) {
        Serial.println("Error: No se pudo realizar el escaneo");
        return;
    }

    if (numSsid == 0) {
        Serial.println("No se encontraron redes WiFi");
        return;
    }

    Serial.print("Redes encontradas: ");
    Serial.println(numSsid);

    for (int i = 0; i < numSsid; i++) {
        Serial.print(i);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(" dBm) ");
        Serial.print("Encriptación: ");
        printEncryptionType(WiFi.encryptionType(i));
        delay(10);  // Pequeña pausa entre cada red
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
        while (WiFi.status() != WL_CONNECTED && intentos < 30) {
            delay(1000);
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
    Serial.begin(115200);

    matrix.loadSequence(logoscroll);
    matrix.begin();
    matrix.autoscroll(1000); // No está funcionando.
    matrix.play(true);

    // Verificar la presencia de la interfaz wifi.
    if (WiFi.status() == WL_NO_SHIELD || WiFi.status() == WL_NO_MODULE) {
        Serial.println("La interfaz Wifi no está disponible.");
    }

    // Comprobar versión del firmaware.
    if (const String fv = WiFi.firmwareVersion(); fv != "0.6.0") {
        Serial.println("Porfavor actualice el firmware de la interfaz Wifi.");
    }

    wifi_connect();
}

/////////////////////
///     LOOP      ///
/////////////////////
void loop() {


    delay(10000);
}
