/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-wi-fi-manager-asyncwebserver/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include "Web.h"
#include <ArduinoJson.h>

AsyncWebServer server(80);

StaticJsonDocument<1024> jsonData;

// Search for parameter in HTTP POST request
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";
const char* PARAM_INPUT_3 = "ip";
const char* PARAM_INPUT_4 = "gateway";

//Variables to save values from HTML form
String ssid;
String pass;
String ip;
String gateway;

// File paths to save input values permanently
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";
const char* ipPath = "/ip.txt";
const char* gatewayPath = "/gateway.txt";

const char* AP_SSID = "USB-Insight-HUB";
const char* AP_PASS = "12345678";

//IPAddress localIP;
IPAddress localIP(192, 168, 7, 106); // hardcoded

// Set your Gateway IP address
//IPAddress localGateway;
IPAddress localGateway(192, 168, 7, 106); //hardcoded
IPAddress subnet(255, 255, 255, 0);

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)

ChannelData channels[] = {
  {"COM2", 5.000, 0.250, 1000, -20, false, true},
  {"COM7", 4.900, 1.520, 1900, -20, true, false},
  {"COM3", 5.030, 0.000, 500, -10, false, true},
};


// Read File from LittleFS
String readFile(fs::FS &fs, const char * path){
  File file = fs.open(path);
  if(!file || file.isDirectory()){
    return String();
  }
  
  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;     
  }
  return fileContent;
}

// Write file to LittleFS
void writeFile(fs::FS &fs, const char * path, const char * message){

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    return;
  }
  
  file.print(message);
}

// Initialize WiFi
bool initWiFi() {
  if(ssid=="" || ip==""){
    return false;
  }

  WiFi.mode(WIFI_STA);
  localIP.fromString(ip.c_str());
  localGateway.fromString(gateway.c_str());

  if (!WiFi.config(localIP, localGateway, subnet)){
    return false;
  }

  WiFi.begin(ssid.c_str(), pass.c_str());

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;

  while(WiFi.status() != WL_CONNECTED) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      return false;
    }
  }

  return true;
}

void InitJsonData() {
  JsonArray channels = jsonData.to<JsonArray>();

  JsonObject channel1 = channels.createNestedObject();
  channel1["enumerator"] = "COM2";
  channel1["voltage"] = 5.000;
  channel1["current"] = 0.250;
  channel1["overCurrentLimit"] = 1000;
  channel1["backCurrentLimit"] = -20;
  channel1["powerControl"] = true;
  channel1["dataControl"] = false;

  JsonObject channel2 = channels.createNestedObject();
  channel2["enumerator"] = "COM7";
  channel2["voltage"] = 4.900;
  channel2["current"] = 1.520;
  channel2["overCurrentLimit"] = 1900;
  channel2["backCurrentLimit"] = -20;
  channel2["powerControl"] = false;
  channel2["dataControl"] = true;

  JsonObject channel3 = channels.createNestedObject();
  channel3["enumerator"] = "COM3";
  channel3["voltage"] = 5.030;
  channel3["current"] = 0.000;
  channel3["overCurrentLimit"] = 500;
  channel3["backCurrentLimit"] = -10;
  channel3["powerControl"] = true;
  channel3["dataControl"] = false;
}

void updateChannelEnumerator(int channelIndex, const char *newEnumerator) {
  if (channelIndex >= 0 && channelIndex < jsonData.size()) {
    // Update the enumerator value in jsonData
    jsonData[channelIndex]["enumerator"] = newEnumerator;
  }
}

void updateChannelVoltage(int channelIndex, float newVoltage) {
  if (channelIndex >= 0 && channelIndex < jsonData.size()) {
    // Update the voltage value in jsonData
    jsonData[channelIndex]["voltage"] = newVoltage;
  }
}

void updateChannelCurrent(int channelIndex, float newCurrent) {
  if (channelIndex >= 0 && channelIndex < jsonData.size()) {
    // Update the current value in jsonData
    jsonData[channelIndex]["current"] = newCurrent;
  }
}

void StartWifi() {
  LittleFS.begin(true);
  
  // Load values saved in LittleFS
  ssid = readFile(LittleFS, ssidPath);
  pass = readFile(LittleFS, passPath);
  ip = readFile(LittleFS, ipPath);
  gateway = readFile (LittleFS, gatewayPath);


  if(initWiFi()) {
    InitJsonData();

    server.on("/getChannels", HTTP_GET, [](AsyncWebServerRequest *request) {
      // Create a String to hold the serialized JSON data
      String json;
      serializeJson(jsonData, json);

      // Send the JSON response
      request->send(200, "application/json", json);
    });

    // Serve the main page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(LittleFS, "/index.html", "text/html");
    });

    // Serve static files (CSS, JS, etc.)
    server.serveStatic("/", LittleFS, "/");
    server.begin();
  }
  else {
    WiFi.softAPConfig(localIP, localGateway, subnet);
    WiFi.softAP(AP_SSID, AP_PASS);

    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(LittleFS, "/wifimanager.html", "text/html");
    });
    
    server.serveStatic("/", LittleFS, "/");
    
    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i=0;i<params;i++){
        const AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            ssid = p->value().c_str();

            // Write file to save value
            writeFile(LittleFS, ssidPath, ssid.c_str());
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) {
            pass = p->value().c_str();

            // Write file to save value
            writeFile(LittleFS, passPath, pass.c_str());
          }
          // HTTP POST ip value
          if (p->name() == PARAM_INPUT_3) {
            ip = p->value().c_str();

            // Write file to save value
            writeFile(LittleFS, ipPath, ip.c_str());
          }
          // HTTP POST gateway value
          if (p->name() == PARAM_INPUT_4) {
            gateway = p->value().c_str();

            // Write file to save value
            writeFile(LittleFS, gatewayPath, gateway.c_str());
          }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: " + ip);
      delay(3000);
      ESP.restart();
    });
    server.begin();
  }
}

