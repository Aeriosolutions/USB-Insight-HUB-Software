/**
 *   ESP32 SvelteKit
 *
 *   A simple, secure and extensible framework for IoT projects for ESP32 platforms
 *   with responsive Sveltekit front-end built with TailwindCSS and DaisyUI.
 *   https://github.com/theelims/ESP32-sveltekit
 *
 *   Copyright (C) 2018 - 2023 rjwats
 *   Copyright (C) 2023 - 2024 theelims
 *
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the LGPL v3 license. See the LICENSE file for details.
 **/

#include <MasterStateService.h>

void logJsonObject(JsonObject &root);

//default json structure
static const char *jsonDefault = R"rawliteral(
{
  "power_on": false,
  "switch_on": false,
  "features_conf_startUpmode": 0,
  "features_conf_wifi_enabled": true,
  "features_conf_hubMode": 0,
  "features_conf_filterType": 1,
  "features_conf_refreshRate": 0,
  "features_startUpActive": 0,
  "features_pcConnected": false,
  "features_vbusVoltage": 0,
  "screen_conf_rotation": 2,
  "screen_conf_brightness": 800,
  "BaseMCU_vext_cc": 0,
  "BaseMCU_vhost_cc": 0,
  "BaseMCU_vext_stat": 0,
  "BaseMCU_vhost_stat": 0,
  "BaseMCU_pwr_source": false,
  "BaseMCU_usb3_mux_out_en": false,
  "BaseMCU_usb3_mux_sel_pos": false,
  "BaseMCU_base_ver": 0,
  "channels": [
    {
      "startup_counter": 0,
      "startup_conf_timer": 0,
      "meter_voltage": 0,
      "meter_current": 0,
      "meter_fwdAlertSet": false,
      "meter_backAlertSet": false,
      "meter_conf_fwdCLim": 1000,
      "meter_conf_backCLim": 20,
      "USBInfo_numDev": 0,
      "USBInfo_Dev1_Name": "",
      "USBInfo_Dev2_Name": "",
      "USBInfo_usbType": 0,
      "BaseMCU_fault": false,
      "BaseMCU_ilim": 1,
      "BaseMCU_data_en": false,
      "BaseMCU_pwr_en": false
    },
    {
      "startup_counter": 0,
      "startup_conf_timer": 0,
      "meter_voltage": 0,
      "meter_current": 0,
      "meter_fwdAlertSet": false,
      "meter_backAlertSet": false,
      "meter_conf_fwdCLim": 1000,
      "meter_conf_backCLim": 20,
      "USBInfo_numDev": 0,
      "USBInfo_Dev1_Name": "",
      "USBInfo_Dev2_Name": "",
      "USBInfo_usbType": 0,
      "BaseMCU_fault": false,
      "BaseMCU_ilim": 1,
      "BaseMCU_data_en": false,
      "BaseMCU_pwr_en": false
    },
    {
      "startup_counter": 0,
      "startup_conf_timer": 0,
      "meter_voltage": 0,
      "meter_current": 0,
      "meter_fwdAlertSet": false,
      "meter_backAlertSet": false,
      "meter_conf_fwdCLim": 1000,
      "meter_conf_backCLim": 20,
      "USBInfo_numDev": 0,
      "USBInfo_Dev1_Name": "",
      "USBInfo_Dev2_Name": "",
      "USBInfo_usbType": 0,
      "BaseMCU_fault": false,
      "BaseMCU_ilim": 1,
      "BaseMCU_data_en": false,
      "BaseMCU_pwr_en": false
    }
  ]
}
)rawliteral";

MasterStateService::MasterStateService(PsychicHttpServer *server,
                                     EventSocket *socket,
                                     SecurityManager *securityManager) :    _httpEndpoint(MasterState::read,
                                                                                            MasterState::update,
                                                                                            this,
                                                                                            server,
                                                                                            LIGHT_SETTINGS_ENDPOINT_PATH,
                                                                                            securityManager,
                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                            _eventEndpoint(MasterState::read,
                                                                                            MasterState::update,
                                                                                            this,
                                                                                            socket,
                                                                                            LIGHT_SETTINGS_EVENT),
                                                                            _webSocketServer(MasterState::read,
                                                                                            MasterState::update,
                                                                                            this,
                                                                                            server,
                                                                                            LIGHT_SETTINGS_SOCKET_PATH,
                                                                                            securityManager,
                                                                                            AuthenticationPredicates::IS_AUTHENTICATED)
{
    // configure led to be output
    ESP_LOGI("MasterState","Setup power_on");

    // configure settings service update handler to update LED state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);
}

void MasterStateService::begin(GlobalState *globalState, GlobalConfig *globalConfig)
{
    pinMode(LED_BUILTIN, OUTPUT);

    gState = globalState;
    gConfig = globalConfig;

    //initialize json document
    deserializeJson(masterStateDoc,jsonDefault);
 
    masterStateObj = masterStateDoc.as<JsonObject>();

    _httpEndpoint.begin();
    _eventEndpoint.begin();
    _state.powerOn = DEFAULT_POWER_STATE;
    _state.switchOn = false;
    
    copyGlobalToBackend(masterStateObj);
    lastHash = calculateJsonHash(masterStateObj);
    update(masterStateObj,MasterState::update,"startup");
    
    onConfigUpdated();
    xTaskCreatePinnedToCore(taskMSSImpl, "Master State Service", 6144, this, 3,NULL,APP_CORE);
}

void MasterStateService::onConfigUpdated()
{
    digitalWrite(LED_BUILTIN, _state.powerOn ? 1 : 0);
    //ESP_LOGI("MasterState","Power Button changed to %s",_state.powerOn ? "on" : "off");
    //ESP_LOGI("MasterState","Switch Input changed to %s",_state.switchOn ? "on" : "off");
}

void MasterStateService::taskMSSImpl(void *pvParameters){
    MasterStateService *instance = static_cast<MasterStateService *>(pvParameters);
    instance->taskMSS(); 
}

void MasterStateService::taskMSS(){
    TickType_t xLastWakeTime = xTaskGetTickCount();
    ESP_LOGI("Master State Service","Started on Core %u",xPortGetCoreID());

    for(;;){
               
        read(masterStateObj,MasterState::read);
        if(lastHash !=calculateJsonHash(masterStateObj)){
          logJsonObject(masterStateObj);
          ESP_LOGI("","c1:%u",_state.chData[0].BaseMCU_pwr_en);
          ESP_LOGI("","c2:%u",_state.chData[1].BaseMCU_pwr_en);
          ESP_LOGI("","c3:%u",_state.chData[2].BaseMCU_pwr_en);
          vTaskDelay(pdMS_TO_TICKS(20));
          //copyBackendToGlobal(masterStateObj);
          ESP_LOGI("","Hash changed");
        }
        //bool power_on = (bool)(masterStateObj["power_on"]);
        //bool switch_on = (bool)(masterStateObj["switch_on"]);
        
        copyGlobalToBackend(masterStateObj);
        //vTaskDelay(pdMS_TO_TICKS(20));
        bool power_on = _state.powerOn;
        bool switch_on = _state.switchOn;
        //ESP_LOGI("","power_on %u", power_on);
        switch_on = !switch_on;
        masterStateObj["switch_on"] = switch_on;
        //_state.switchOn = switch_on;
        update(masterStateObj,MasterState::update,"main");
        lastHash = calculateJsonHash(masterStateObj);
        ESP_LOGI("","Hash: %u", lastHash);
        //logJsonObject(masterStateObj);

        vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(FRONTEND_UPDATE_PERIOD));
    }
}

void MasterStateService::copyBackendToGlobal(JsonObject &root){       

  //commented values are read only from the Backend
  gConfig->features.startUpmode = root["features_conf_startUpmode"].as<uint8_t>();
  gConfig->features.wifi_enabled = root["features_conf_wifi_enabled"].as<uint8_t>();
  gConfig->features.hubMode = root["features_conf_hubMode"].as<uint8_t>();
  gConfig->features.filterType = root["features_conf_filterType"].as<uint8_t>();
  gConfig->features.refreshRate = root["features_conf_refreshRate"].as<uint8_t>();
  //gState->features.startUpActive = root["features_startUpActive"] | false;
  //gState->features.pcConnected = root["features_pcConnected"] | false;
  //gState->features.vbusVoltage = root["features_vbusVoltage"].as<float>();
  gConfig->screen[0].rotation = root["screen_conf_rotation"].as<uint8_t>();
  gConfig->screen[0].brightness = root["screen_conf_brightness"].as<uint16_t>();
  //gState->baseMCUExtra.vext_cc = root["BaseMCU_vext_cc"].as<uint8_t>();
  //gState->baseMCUExtra.vhost_cc = root["BaseMCU_vhost_cc"].as<uint8_t>();
  //gState->baseMCUExtra.vext_stat = root["BaseMCU_vext_stat"].as<uint8_t>();
  //gState->baseMCUExtra.vhost_stat = root["BaseMCU_vhost_stat"].as<uint8_t>();
  //gState->baseMCUExtra.pwr_source = root["BaseMCU_pwr_source"] | false;
  //gState->baseMCUExtra.usb3_mux_out_en = root["BaseMCU_usb3_mux_out_en"] | false;
  //gState->baseMCUExtra.usb3_mux_sel_pos = root["BaseMCU_usb3_mux_sel_pos"] | false;
  //gState->baseMCUExtra.base_ver = root["BaseMCU_base_ver"].as<uint8_t>();                

  for(int i =0; i<3; i++){
      JsonObject ch = root["channels"][i];
      //gState->startup[i].startup_cnt = ch["startup_counter"].as<int>();
      gConfig->startup[i].startup_timer = ch["startup_conf_timer"].as<int>();
      //gState->meter[i].AvgVoltage = ch["meter_voltage"].as<float>();
      //gState->meter[i].AvgCurrent = ch["meter_current"].as<float>();
      //gState->meter[i].fwdAlertSet = ch["meter_fwdAlertSet"] | false;
      //gState->meter[i].backAlertSet = ch["meter_backAlertSet" ] | false;
      gConfig->meter[i].fwdCLim = ch["meter_conf_fwdCLim"].as<uint16_t>();
      gConfig->meter[i].backCLim = ch["meter_conf_backCLim"].as<uint16_t>();
      //gState->usbInfo[i].numDev = ch["USBInfo_numDev"].as<int>();
      //gState->usbInfo[i].Dev1_Name = ch["USBInfo_Dev1_Name"].as<String>();
      //gState->usbInfo[i].Dev2_Name = ch["USBInfo_Dev2_Name"].as<String>();
      //gState->usbInfo[i].usbType = ch["USBInfo_usbType"].as<int>();
      //gState->baseMCUIn[i].fault = ch["BaseMCU_fault"] | false;
      //gState->baseMCUOut[i].ilim = ch["BaseMCU_ilim"].as<uint8_t>();
      gState->baseMCUOut[i].data_en = ch["BaseMCU_data_en"] | false;
      gState->baseMCUOut[i].pwr_en = ch["BaseMCU_pwr_en"] | false;
  }
}

void MasterStateService::copyGlobalToBackend(JsonObject &root){

  //move MasterState data to Stateles json structures
  root["features_conf_startUpmode"] = gConfig->features.startUpmode;
  root["features_conf_wifi_enabled"] = gConfig->features.wifi_enabled;
  root["features_conf_hubMode"] = gConfig->features.hubMode;
  root["features_conf_filterType"] = gConfig->features.filterType;
  root["features_conf_refreshRate"] = gConfig->features.refreshRate;
  root["features_startUpActive"] = gState->features.startUpActive;
  root["features_pcConnected"] = gState->features.pcConnected;
  root["features_vbusVoltage"] = gState->features.vbusVoltage;
  root["screen_conf_rotation"] = gConfig->screen[0].rotation;
  root["screen_conf_brightness"] = gConfig->screen[0].brightness;
  root["BaseMCU_vext_cc"] = gState->baseMCUExtra.vext_cc;
  root["BaseMCU_vhost_cc"] = gState->baseMCUExtra.vhost_cc;
  root["BaseMCU_vext_stat"] = gState->baseMCUExtra.vext_stat;
  root["BaseMCU_vhost_stat"] = gState->baseMCUExtra.vhost_stat;
  root["BaseMCU_pwr_source"] = gState->baseMCUExtra.pwr_source;
  root["BaseMCU_usb3_mux_out_en"] =  gState->baseMCUExtra.usb3_mux_out_en;
  root["BaseMCU_usb3_mux_sel_pos"] = gState->baseMCUExtra.usb3_mux_sel_pos;
  root["BaseMCU_base_ver"] = gState->baseMCUExtra.base_ver;

  JsonArray channels = root["channels"].as<JsonArray>();

  for(int i =0; i<3; i++){
      JsonObject ch = root["channels"][i].as<JsonObject>();
      ch["startup_counter"] = gState->startup[i].startup_cnt;
      ch["startup_conf_timer"] = gConfig->startup[i].startup_timer;
      ch["meter_voltage"] = gState->meter[i].AvgVoltage;
      ch["meter_current"] = gState->meter[i].AvgCurrent;
      ch["meter_fwdAlertSet"] = gState->meter[i].fwdAlertSet;
      ch["meter_backAlertSet"] = gState->meter[i].backAlertSet;
      ch["meter_conf_fwdCLim"] = gConfig->meter[i].fwdCLim;
      ch["meter_conf_backCLim"] = gConfig->meter[i].backCLim;
      ch["USBInfo_numDev"] = gState->usbInfo[i].numDev;
      ch["USBInfo_Dev1_Name"] = gState->usbInfo[i].Dev1_Name;
      ch["USBInfo_Dev2_Name"] = gState->usbInfo[i].Dev2_Name;
      ch["USBInfo_usbType"] = gState->usbInfo[i].usbType;
      ch["BaseMCU_fault"] = gState->baseMCUIn[i].fault;
      ch["BaseMCU_ilim"] = gState->baseMCUOut[i].ilim;
      ch["BaseMCU_data_en"] = gState->baseMCUOut[i].data_en;
      ch["BaseMCU_pwr_en"] = gState->baseMCUOut[i].pwr_en;
  }

  //logJsonObject(root);
}

void logJsonObject(JsonObject &root) {
    // Create a temporary buffer to hold JSON output
    char buffer[2048];  // Adjust size if needed

    // Serialize JSON into the buffer
    size_t jsonSize = serializeJson(root, buffer, sizeof(buffer));
    //ESP_LOGI("JSON_LOG", "JSON Size: %d", jsonSize);
    if (jsonSize == 0) {
        ESP_LOGE("JSON_LOG", "Failed to serialize JSON");
        return;
    }

    // Log JSON output
    ESP_LOGI("JSON_LOG", "JSON Output: %s", buffer);
}

uint32_t MasterStateService::calculateJsonHash(JsonObject &root) {
    // Serialize JSON to a temporary buffer
    char buffer[2048];  // Adjust buffer size if needed
    size_t jsonSize = serializeJson(root, buffer, sizeof(buffer));

    if (jsonSize == 0) {
        return 0;  // Serialization failed
    }

    // Compute CRC32 hash
    return esp_crc32_le(0, (const uint8_t*)buffer, jsonSize);
}