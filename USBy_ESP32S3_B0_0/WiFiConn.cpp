//Add licence text
#include <WiFi.h>
#include "WifiConn.h"


const char* ssid = "MyNetNew";
const char* password = "12345678";

IPAddress local_ip(192, 168, 7, 106);
IPAddress gateway(192, 168, 7, 106);
IPAddress subnet(255, 255, 255, 0);

void StartWifi() {
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(ssid, password);
}

