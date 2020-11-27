#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <EEPROM.h>
#include "jsonHandler.h"
#include "handleHttp.h"

struct Function{
    char* pinMode;
    bool isAnalog;
    bool isPWM;
    int values[];
    char mqttCmds[];
    
    Function(){
        pinMode = "OUTPUT";
        isPWM = false;
        isAnalog = false;
        values[0] = 0;
        values[1] = 1;
        mqttCmds[0] = 0;
        mqttCmds[1] = 1;
    }


    Function(char* pinMode){
        isPWM = false;
        isAnalog = false;
        values[0] = 0;
        values[1] = 1;
        mqttCmds[0] = 0;
        mqttCmds[1] = 1;
    }

    Function(char* pinMode, bool isAnalog, bool isPWM, int values[], char mqttCmds[]);
};

Function functions[] = {Function(), Function("Input")};

struct Port{
  char* pin;
  Function type;

  Port(char* port, int function){
    pin = port;
    type = functions[function];
  };
};

Port configuredPorts[] = {};

void parsePorts(String rawJSON){
    const size_t capacity = JSON_ARRAY_SIZE(64) + JSON_OBJECT_SIZE(1) + 64*JSON_OBJECT_SIZE(2) + 40;
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc, rawJSON);
    JsonArray ports = doc["ports"];
    int next;

    for (int i = 0; i < 64; i++){
        if(ports[i]["port"]!= "" ){
            configuredPorts[i].pin = ports[i]["port"];
            configuredPorts[i].type = functions[int(ports[i]["function"])];
        }else next = i;
    }
}

String encodePorts(){
    const size_t capacity = JSON_ARRAY_SIZE(64) + JSON_OBJECT_SIZE(1) + 64*JSON_OBJECT_SIZE(2);
    DynamicJsonDocument doc(capacity);

    JsonArray ports = doc.createNestedArray("ports");

    JsonObject ports_0 = ports.createNestedObject();
    ports_0["port"] = "D1";
    ports_0["function"] = 1;

    for (int i = 0; i < 64; i++)
    {
        ports[i]["port"]=configuredPorts[i].pin;
        ports[i]["function"]= getPosition(configuredPorts[i].type);
    }
    serializeJson(doc, Serial);
}

int getPosition(Function p){
    int posiition;
    for (int i=0; i<sizeof(functions); i++) {
        if (p.pinMode == functions[i].pinMode && p.isAnalog == functions[i].isAnalog && p.isPWM == functions[i].isPWM && p.values == functions[i].values && p.mqttCmds == functions[i].mqttCmds) {
            posiition = i;
            break;
        }
    }
}

void loadData(){
    EEPROM.begin(512);
    EEPROM.get(0, ssid);
    EEPROM.get(0 + sizeof(ssid), password);
    char ok[2 + 1];
    EEPROM.get(0 + sizeof(ssid) + sizeof(password), ok);
    EEPROM.end();
    if (String(ok) != String("OK")) {
        ssid[0] = 0;
        password[0] = 0;
        firstBoot = true;
    }
    Serial.println("Recovered credentials:");
    Serial.println(ssid);
    Serial.println(strlen(password) > 0 ? "********" : "<no password>");
    LittleFS.begin();
    File g = LittleFS.open("/ports.txt", "r");
    if(g) parsePorts(g.readString());
    Serial.println("Finished loading data")
}

void saveData() {
    EEPROM.begin(512);
    EEPROM.put(0, ssid);
    EEPROM.put(0 + sizeof(ssid), password);
    char ok[2 + 1] = "OK";
    EEPROM.put(0 + sizeof(ssid) + sizeof(password), ok);
    EEPROM.commit();
    EEPROM.end();
    Serial.println("Saved wifi credentials");
}