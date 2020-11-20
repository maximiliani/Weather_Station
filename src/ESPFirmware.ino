#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <LittleFS.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include "jsonHandler.cpp"

const char* ssid;
const char* password;

bool firstBoot(){
    LittleFS.begin();
    File st = LittleFS.open("/start.txt", "r");
    if (st){
        File s = LittleFS.open("/start.txt", "r");
        ssid = s.readString().c_str();
        File p = LittleFS.open("/pass.txt", "r");
        if (p) password = s.readString().c_str();
        File g = LittleFS.open("/ports.txt", "r");
        if (g) parsePorts(g.readString().c_str());
        return false;
        }
    return true;
}

void showCaptive(){

}

void wifiSetup(){
    if (firstBoot) showCaptive();
    else if (LittleFS.open("/wifiMode.txt", "r").readString().equals("captive")) showCaptive();
    else {
        WiFi.begin();
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
    }
}

void setup(){
    Serial.begin(115200);
    wifiSetup();
}

void loop(){
    Serial.print("hi");
}