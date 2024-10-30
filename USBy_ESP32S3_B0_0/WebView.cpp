#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "WebView.h"

AsyncWebServer server(80);

static ChannelData channel1 = {Channel_1, "COM2", 5.001, 0.250, 1000, -20, false, false};
static ChannelData channel2 = {Channel_2, "COM7", 4.902, 1.523, 1900, -20, true, false};
static ChannelData channel3 = {Channel_3, "COM3", 5.025, 0.000, 500, -10, false, true};

ChannelData& getChannel(eChannel channelNumber) {
  switch(channelNumber) {
    case Channel_1: return channel1;
    case Channel_2: return channel2;
    case Channel_3: return channel3;
    default: return channel1;
  }
}

// Standalone setters
void setChannelEnumerator(eChannel channelNumber, const String& newEnumerator) {
  getChannel(channelNumber).setEnumerator(newEnumerator);
}

void setChannelVoltage(eChannel channelNumber, float newVoltage) {
  getChannel(channelNumber).setVoltage(newVoltage);
}

void setChannelCurrent(eChannel channelNumber, float newCurrent) {
  getChannel(channelNumber).setCurrent(newCurrent);
}

void setChannelOverCurrentLimit(eChannel channelNumber, int newLimit) {
  getChannel(channelNumber).setOverCurrentLimit(newLimit);
}

void setChannelBackCurrentLimit(eChannel channelNumber, int newLimit) {
  getChannel(channelNumber).setBackCurrentLimit(newLimit);
}

void setChannelPowerControl(eChannel channelNumber, bool control) {
  getChannel(channelNumber).setPowerControl(control);
}

void setChannelDataControl(eChannel channelNumber, bool control) {
  getChannel(channelNumber).setDataControl(control);
}

// Standalone getters
String getChannelEnumerator(eChannel channelNumber) {
  return getChannel(channelNumber).getEnumerator();
}

float getChannelVoltage(eChannel channelNumber) {
  return getChannel(channelNumber).getVoltage();
}

float getChannelCurrent(eChannel channelNumber) {
  return getChannel(channelNumber).getCurrent();
}

int getChannelOverCurrentLimit(eChannel channelNumber) {
  return getChannel(channelNumber).getOverCurrentLimit();
}

int getChannelBackCurrentLimit(eChannel channelNumber) {
  return getChannel(channelNumber).getBackCurrentLimit();
}

bool getChannelPowerControl(eChannel channelNumber) {
  return getChannel(channelNumber).getPowerControl();
}

bool getChannelDataControl(eChannel channelNumber) {
  return getChannel(channelNumber).getDataControl();
}

// Function to generate the HTML for each channel
String generateChannelHTML(const ChannelData& channel, int channelNumber) {
  String html = "<div class='channel'>";
  html += "<div class='title'>Channel " + String(channelNumber) + "</div>";
  html += "<div class='row'>Enumerator: <input type='text' id='channel" + String(channelNumber) + "-enumerator' value='" + channel.enumerator + "' readonly></div>";
  html += "<div class='row'>Voltage: <span id='channel" + String(channelNumber) + "-voltage'>" + String(channel.voltage, 3) + " V</span></div>";
  html += "<div class='row'>Current: <span id='channel" + String(channelNumber) + "-current'>" + String(channel.current, 3) + " A</span> <sup>[1]</sup></div>";
  html += "<div class='row'>Over Current limit: <input type='number' value='" + String(channel.overCurrentLimit) + "'> mA</div>";
  html += "<div class='row'>Back Current limit: <input type='number' value='" + String(channel.backCurrentLimit) + "'> mA</div>";
  html += "<div class='row toggle'>Power Control: <input type='checkbox' ";
  html += (channel.powerControl ? "checked" : "");
  html += "></div>";
  html += "<div class='row toggle'>Data Control: <input type='checkbox' ";
  html += (channel.dataControl ? "checked" : "");
  html += "></div>";
  html += "</div>";
  return html;
}

void generateWebPage(AsyncWebServerRequest *request, ChannelData channel1, ChannelData channel2, ChannelData channel3) {
  String html = "<!DOCTYPE html><html><head><title>USB Insight Hub Control Panel</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; }";
  html += "h2 { color: orange; font-size: 24px; }";
  html += ".container { display: flex; gap: 20px; justify-content: center; }";
  html += ".channel { border: 1px solid #000; padding: 15px; width: 250px; }";
  html += ".title { font-weight: bold; font-size: 18px; margin-bottom: 10px; }";
  html += ".row { margin-bottom: 8px; }";
  html += ".toggle { display: flex; align-items: center; gap: 5px; }";
  html += "input[type='text'], input[type='number'] { width: 60px; }";
  html += ".green-text { color: green; }";
  html += ".switch { position: relative; display: inline-block; width: 34px; height: 20px; }";
  html += ".switch input { opacity: 0; width: 0; height: 0; }";
  html += ".slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; transition: .4s; }";
  html += ".slider:before { position: absolute; content: ''; height: 14px; width: 14px; left: 3px; bottom: 3px; background-color: white; transition: .4s; }";
  html += "input:checked + .slider { background-color: blue; }";
  html += "input:checked + .slider:before { transform: translateX(14px); }";
  html += ".slider.round { border-radius: 34px; }";
  html += ".slider.round:before { border-radius: 50%; }";
  html += "</style>";

  // JavaScript to fetch updated data every 0.5 seconds
html += "<script>";
html += "function updateData() {";
html += "  fetch('/update').then(response => response.json()).then(data => {";
html += "    document.getElementById('channel1-voltage').innerText = data.channel1.voltage.toFixed(3) + ' V';";
html += "    document.getElementById('channel1-current').innerText = data.channel1.current.toFixed(3) + ' A';";
html += "    document.getElementById('channel1-enumerator').innerText = data.channel1.enumerator;";

html += "    document.getElementById('channel2-voltage').innerText = data.channel2.voltage.toFixed(3) + ' V';";
html += "    document.getElementById('channel2-current').innerText = data.channel2.current.toFixed(3) + ' A';";
html += "    document.getElementById('channel2-enumerator').innerText = data.channel2.enumerator;";

html += "    document.getElementById('channel3-voltage').innerText = data.channel3.voltage.toFixed(3) + ' V';";
html += "    document.getElementById('channel3-current').innerText = data.channel3.current.toFixed(3) + ' A';";
html += "    document.getElementById('channel3-enumerator').innerText = data.channel3.enumerator;";
html += "  });";
html += "}";
html += "setInterval(updateData, 500);"; // Update every 500ms
html += "</script>";


  html += "</head><body>";
  html += "<h2>USB Insight Hub Control Panel</h2>";
  html += "<div class='container'>";

  // Channel 1
  html += "<div class='channel'>";
  html += "<div class='title'>Channel 1</div>";
  html += "<div class='row'>Enumerator: <input type='text' value='" + channel1.enumerator + "' readonly></div>";
  html += "<div class='row'>Voltage: <span id='channel1-voltage' class='green-text'>" + String(channel1.voltage, 3) + " V</span></div>";
  html += "<div class='row'>Current: <span id='channel1-current' class='green-text'>" + String(channel1.current, 3) + " A</span></div>";
  html += "<div class='row'>Over Current limit: <input type='number' value='" + String(channel1.overCurrentLimit) + "'> mA</div>";
  html += "<div class='row'>Back Current limit: <input type='number' value='" + String(channel1.backCurrentLimit) + "'> mA</div>";
  html += "<div class='row toggle'>Power Control: <label class='switch'><input type='checkbox' " + String(channel1.powerControl ? "checked" : "") + "><span class='slider round'></span></label></div>";
  html += "<div class='row toggle'>Data Control: <label class='switch'><input type='checkbox' " + String(channel1.dataControl ? "checked" : "") + "><span class='slider round'></span></label></div>";
  html += "</div>";

  // Channel 2
  html += "<div class='channel'>";
  html += "<div class='title'>Channel 2</div>";
  html += "<div class='row'>Enumerator: <input type='text' value='" + channel2.enumerator + "' readonly></div>";
  html += "<div class='row'>Voltage: <span id='channel2-voltage' class='green-text'>" + String(channel2.voltage, 3) + " V</span></div>";
  html += "<div class='row'>Current: <span id='channel2-current' class='green-text'>" + String(channel2.current, 3) + " A</span></div>";
  html += "<div class='row'>Over Current limit: <input type='number' value='" + String(channel2.overCurrentLimit) + "'> mA</div>";
  html += "<div class='row'>Back Current limit: <input type='number' value='" + String(channel2.backCurrentLimit) + "'> mA</div>";
  html += "<div class='row toggle'>Power Control: <label class='switch'><input type='checkbox' " + String(channel2.powerControl ? "checked" : "") + "><span class='slider round'></span></label></div>";
  html += "<div class='row toggle'>Data Control: <label class='switch'><input type='checkbox' " + String(channel2.dataControl ? "checked" : "") + "><span class='slider round'></span></label></div>";
  html += "</div>";

  // Channel 3
  html += "<div class='channel'>";
  html += "<div class='title'>Channel 3</div>";
  html += "<div class='row'>Enumerator: <input type='text' value='" + channel3.enumerator + "' readonly></div>";
  html += "<div class='row'>Voltage: <span id='channel3-voltage' class='green-text'>" + String(channel3.voltage, 3) + " V</span></div>";
  html += "<div class='row'>Current: <span id='channel3-current' class='green-text'>" + String(channel3.current, 3) + " A</span></div>";
  html += "<div class='row'>Over Current limit: <input type='number' value='" + String(channel3.overCurrentLimit) + "'> mA</div>";
  html += "<div class='row'>Back Current limit: <input type='number' value='" + String(channel3.backCurrentLimit) + "'> mA</div>";
  html += "<div class='row toggle'>Power Control: <label class='switch'><input type='checkbox' " + String(channel3.powerControl ? "checked" : "") + "><span class='slider round'></span></label></div>";
  html += "<div class='row toggle'>Data Control: <label class='switch'><input type='checkbox' " + String(channel3.dataControl ? "checked" : "") + "><span class='slider round'></span></label></div>";
  html += "</div>";

  html += "</div></body></html>";

  request->send(200, "text/html", html);
}


// Function to initialize the server and serve the web page
void StartWebView() {
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  generateWebPage(request, channel1, channel2, channel3);  // Use global instances directly
});

server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
  // Directly access global instances
  String json = "{";
  json += "\"channel1\":{\"enumerator\":\"" + channel1.enumerator + "\",\"voltage\":" + String(channel1.voltage) + ",\"current\":" + String(channel1.current) + "},";
  json += "\"channel2\":{\"enumerator\":\"" + channel2.enumerator + "\",\"voltage\":" + String(channel2.voltage) + ",\"current\":" + String(channel2.current) + "},";
  json += "\"channel3\":{\"enumerator\":\"" + channel3.enumerator + "\",\"voltage\":" + String(channel3.voltage) + ",\"current\":" + String(channel3.current) + "}";
  json += "}";
  request->send(200, "application/json", json);
});

  server.begin();
}