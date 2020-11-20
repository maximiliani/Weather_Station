#include <ArduinoJson.h>

struct Function{
    char* pinMode;
    bool isAnalog;
    bool isPWM;
    int values[];
    char mqttCmds[];
    
    Function(){
        pinMode = "default";
        isPWM = false;
        isAnalog = false;
        values[0] = 0;
        values[1] = 1;
        mqttCmds[0] = 0;
        mqttCmds[1] = 1;
    };


    Function(char* pinMode){
        isPWM = false;
        isAnalog = false;
        values[0] = 0;
        values[1] = 1;
        mqttCmds[0] = 0;
        mqttCmds[1] = 1;
    };

    Function(char pinMode, bool isAnalog, bool isPWM, int values[], char mqttCmds[]);
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

int getPosition(Function p){
    for (int i=0; i<sizeof(functions); i++) {
        if (p.pinMode == functions[i].pinMode && p.isAnalog == functions[i].isAnalog && p.isPWM == functions[i].isPWM && p.values == functions[i].values && p.mqttCmds == functions[i].mqttCmds) {
            return i;
        }
    }
    return 0;
}

void parsePorts(String rawJSON){
    const size_t capacity = JSON_ARRAY_SIZE(64) + JSON_OBJECT_SIZE(1) + 64*JSON_OBJECT_SIZE(2) + 40;
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc, rawJSON);
    JsonArray ports = doc["ports"];
    int next = 0;

    for (int i = 0; i < 64; i++){
        if(ports[i]["port"]!= "" ){
            configuredPorts[next].pin = ports[i]["port"];
            configuredPorts[next].type = functions[int(ports[i]["function"])];
            next ++;
        }
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
        ports[i]["port"]= configuredPorts[i].pin;
        ports[i]["function"]= getPosition(configuredPorts[i].type);
    }
    String result;
    serializeJson(doc, result);
    return result;
}

