

#include "MasterStateService.h"

void logJsonObject(JsonObject &root);


//default json structure. 
//Not necessary as json is populated from Global state at initialization
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
  "c1_startup_counter": 0,
  "c1_startup_conf_timer": 0,
  "c1_meter_voltage": 0,
  "c1_meter_current": 0,
  "c1_meter_fwdAlertSet": false,
  "c1_meter_backAlertSet": false,
  "c1_meter_conf_fwdCLim": 1000,
  "c1_meter_conf_backCLim": 20,
  "c1_USBInfo_numDev": 0,
  "c1_USBInfo_Dev1_Name": "",
  "c1_USBInfo_Dev2_Name": "",
  "c1_USBInfo_usbType": 0,
  "c1_BaseMCU_fault": false,
  "c1_BaseMCU_ilim": 1,
  "c1_BaseMCU_data_en": false,
  "c1_BaseMCU_pwr_en": false,
  "c2_startup_counter": 0,
  "c2_startup_conf_timer": 0,
  "c2_meter_voltage": 0,
  "c2_meter_current": 0,
  "c2_meter_fwdAlertSet": false,
  "c2_meter_backAlertSet": false,
  "c2_meter_conf_fwdCLim": 1000,
  "c2_meter_conf_backCLim": 20,
  "c2_USBInfo_numDev": 0,
  "c2_USBInfo_Dev1_Name": "",
  "c2_USBInfo_Dev2_Name": "",
  "c2_USBInfo_usbType": 0,
  "c2_BaseMCU_fault": false,
  "c2_BaseMCU_ilim": 1,
  "c2_BaseMCU_data_en": false,
  "c2_BaseMCU_pwr_en": false,
  "c3_startup_counter": 0,
  "c3_startup_conf_timer": 0,
  "c3_meter_voltage": 0,
  "c3_meter_current": 0,
  "c3_meter_fwdAlertSet": false,
  "c3_meter_backAlertSet": false,
  "c3_meter_conf_fwdCLim": 1000,
  "c3_meter_conf_backCLim": 20,
  "c3_USBInfo_numDev": 0,
  "c3_USBInfo_Dev1_Name": "",
  "c3_USBInfo_Dev2_Name": "",
  "c3_USBInfo_usbType": 0,
  "c3_BaseMCU_fault": false,
  "c3_BaseMCU_ilim": 1,
  "c3_BaseMCU_data_en": false,
  "c3_BaseMCU_pwr_en": false
}
)rawliteral";


MasterStateService::MasterStateService(PsychicHttpServer *server,
                                     EventSocket *socket,
                                     SecurityManager *securityManager) :    
                                      _httpEndpoint(MasterState::read,
                                                      MasterState::update,
                                                      this,
                                                      server,
                                                      MASTER_STATE_ENDPOINT_PATH,
                                                      securityManager,
                                                      AuthenticationPredicates::IS_AUTHENTICATED),
                                      _eventEndpoint(MasterState::read,
                                                      MasterState::update,
                                                      this,
                                                      socket,
                                                      MASTER_STATE_EVENT),
                                      _webSocketServer(MasterState::read,
                                                      MasterState::update,
                                                      this,
                                                      server,
                                                      MASTER_STATE_SOCKET_PATH,
                                                      securityManager,
                                                      AuthenticationPredicates::IS_AUTHENTICATED)
{
    
    ESP_LOGI("MasterState","Setup power_on");
    // configure settings service update handler to update LED state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);
}

void MasterStateService::begin(GlobalState *globalState, GlobalConfig *globalConfig, ESP32SvelteKit *esp32sveltekit)
{

    gState = globalState;
    gConfig = globalConfig;
    _skit = esp32sveltekit;

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
    gState->system.APSSID = SettingValue::format("USB-Insight-Hub-#{unique_id}");
    xTaskCreatePinnedToCore(taskMSSImpl, "Master State Service", 9216, this, 3,NULL,APP_CORE);
}

void MasterStateService::onConfigUpdated(){

}

void MasterStateService::taskMSSImpl(void *pvParameters){
    MasterStateService *instance = static_cast<MasterStateService *>(pvParameters);
    instance->taskMSS(); 
}

void MasterStateService::taskMSS(){
    TickType_t xLastWakeTime = xTaskGetTickCount();
    ESP_LOGI("Master State Service","Started on Core %u",xPortGetCoreID());
    unsigned long fallBackTimer = 0;    
    for(;;){
               
        read(masterStateObj,MasterState::read);
        if(lastHash !=calculateJsonHash(masterStateObj)){
          copyBackendToGlobal(masterStateObj);          
        }        
        copyGlobalToBackend(masterStateObj);
        //logJsonObject(masterStateObj);
        //vTaskDelay(pdMS_TO_TICKS(20));
        bool power_on = _state.powerOn;
        bool switch_on = _state.switchOn;
        switch_on = !switch_on;
        masterStateObj["switch_on"] = switch_on;
        //_state.switchOn = switch_on;
        update(masterStateObj,MasterState::update,"main");
        lastHash = calculateJsonHash(masterStateObj);        
        //ESP_LOGI("","Hash: %u",lastHash);
        getNetworkInfo();

        if(millis()-fallBackTimer > FALLBACK_TIMER){
          fallBackTimer = millis();
          wifiFallBackCheck();
        }
        if(gState->features.wifiRecovery == 1){
          _skit->recoveryMode();
          vTaskDelay(pdMS_TO_TICKS(200));
          gState->features.wifiRecovery = 0;
        }
        if(gState->features.wifiReset == 1){
          _skit->factoryReset();
          vTaskDelay(pdMS_TO_TICKS(200));
          gState->features.wifiReset = 0;
        }        
        
        vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(FRONTEND_UPDATE_PERIOD));
    }
}

void MasterStateService::copyBackendToGlobal(JsonObject &root){       

  //commented values are read only from the Backend
  gConfig->features.startUpmode   = root["features_conf_startUpmode"].as<uint8_t>();
  gConfig->features.wifi_enabled  = root["features_conf_wifi_enabled"].as<uint8_t>();
  gConfig->features.hubMode       = root["features_conf_hubMode"].as<uint8_t>();
  gConfig->features.filterType    = root["features_conf_filterType"].as<uint8_t>();
  gConfig->features.refreshRate   = root["features_conf_refreshRate"].as<uint8_t>();
  //gState->features.startUpActive = root["features_startUpActive"] | false;
  //gState->features.pcConnected  = root["features_pcConnected"] | false;
  //gState->features.vbusVoltage  = root["features_vbusVoltage"].as<float>();
  gConfig->screen[0].rotation     = root["screen_conf_rotation"].as<uint8_t>();
  gConfig->screen[0].brightness   = root["screen_conf_brightness"].as<uint16_t>();
  gConfig->screen[1].rotation     = root["screen_conf_rotation"].as<uint8_t>();
  gConfig->screen[1].brightness   = root["screen_conf_brightness"].as<uint16_t>();
  gConfig->screen[2].rotation     = root["screen_conf_rotation"].as<uint8_t>();
  gConfig->screen[2].brightness   = root["screen_conf_brightness"].as<uint16_t>();    
  //gState->baseMCUExtra.vext_cc  = root["BaseMCU_vext_cc"].as<uint8_t>();
  //gState->baseMCUExtra.vhost_cc = root["BaseMCU_vhost_cc"].as<uint8_t>();
  //gState->baseMCUExtra.vext_stat = root["BaseMCU_vext_stat"].as<uint8_t>();
  //gState->baseMCUExtra.vhost_stat = root["BaseMCU_vhost_stat"].as<uint8_t>();
  //gState->baseMCUExtra.pwr_source = root["BaseMCU_pwr_source"] | false;
  //gState->baseMCUExtra.usb3_mux_out_en = root["BaseMCU_usb3_mux_out_en"] | false;
  //gState->baseMCUExtra.usb3_mux_sel_pos = root["BaseMCU_usb3_mux_sel_pos"] | false;
  //gState->baseMCUExtra.base_ver = root["BaseMCU_base_ver"].as<uint8_t>();                

  for(int i =0; i<3; i++){
      
    //gState->startup[i].startup_cnt  = root["c"+String(i+1)+"_startup_counter"].as<int>();
    gConfig->startup[i].startup_timer = root["c"+String(i+1)+"_startup_conf_timer"].as<int>();
    //gState->meter[i].AvgVoltage     = root["c"+String(i+1)+"_meter_voltage"].as<float>();
    //gState->meter[i].AvgCurrent     = root["c"+String(i+1)+"_meter_current"].as<float>();
    gState->meter[i].fwdAlertSet    = root["c"+String(i+1)+"_meter_fwdAlertSet"] | false;
    gState->meter[i].backAlertSet   = root["c"+String(i+1)+"_meter_backAlertSet" ] | false;
    gConfig->meter[i].fwdCLim         = root["c"+String(i+1)+"_meter_conf_fwdCLim"].as<uint16_t>();
    gConfig->meter[i].backCLim        = root["c"+String(i+1)+"_meter_conf_backCLim"].as<uint16_t>();
    //gState->usbInfo[i].numDev       = root["c"+String(i+1)+"_USBInfo_numDev"].as<int>();
    //gState->usbInfo[i].Dev1_Name    = root["c"+String(i+1)+"_USBInfo_Dev1_Name"].as<String>();
    //gState->usbInfo[i].Dev2_Name    = root["c"+String(i+1)+"_USBInfo_Dev2_Name"].as<String>();
    //gState->usbInfo[i].usbType      = root["c"+String(i+1)+"_USBInfo_usbType"].as<int>();
    //gState->baseMCUIn[i].fault      = root["c"+String(i+1)+"_BaseMCU_fault"] | false;
    //gState->baseMCUOut[i].ilim      = root["c"+String(i+1)+"_BaseMCU_ilim"].as<uint8_t>();
    gState->baseMCUOut[i].data_en     = root["c"+String(i+1)+"_BaseMCU_data_en"] | false;
    gState->baseMCUOut[i].pwr_en      = root["c"+String(i+1)+"_BaseMCU_pwr_en"] | false;
  }
}

void MasterStateService::copyGlobalToBackend(JsonObject &root){

  //move MasterState data to Stateles json structures
  root["features_conf_startUpmode"] = gConfig->features.startUpmode;
  root["features_conf_wifi_enabled"] = gConfig->features.wifi_enabled;
  root["features_conf_hubMode"]     = gConfig->features.hubMode;
  root["features_conf_filterType"]  = gConfig->features.filterType;
  root["features_conf_refreshRate"] = gConfig->features.refreshRate;
  root["features_startUpActive"]    = gState->features.startUpActive;
  root["features_pcConnected"]      = gState->features.pcConnected;
  root["features_vbusVoltage"]      = gState->features.vbus;
  root["screen_conf_rotation"]      = gConfig->screen[0].rotation;
  root["screen_conf_brightness"]    = gConfig->screen[0].brightness;
  root["BaseMCU_vext_cc"]           = gState->baseMCUExtra.vext_cc;
  root["BaseMCU_vhost_cc"]          = gState->baseMCUExtra.vhost_cc;
  root["BaseMCU_vext_stat"]         = gState->baseMCUExtra.vext_stat;
  root["BaseMCU_vhost_stat"]        = gState->baseMCUExtra.vhost_stat;
  root["BaseMCU_pwr_source"]        = gState->baseMCUExtra.pwr_source;
  root["BaseMCU_usb3_mux_out_en"]   = gState->baseMCUExtra.usb3_mux_out_en;
  root["BaseMCU_usb3_mux_sel_pos"]  = gState->baseMCUExtra.usb3_mux_sel_pos;
  root["BaseMCU_base_ver"]          = gState->baseMCUExtra.base_ver;

  
  for(int i =0; i<3; i++){
      
    root["c"+String(i+1)+"_startup_counter"]    = gState->startup[i].startup_cnt;
    root["c"+String(i+1)+"_startup_conf_timer"] = gConfig->startup[i].startup_timer;
    root["c"+String(i+1)+"_meter_voltage"]      = gState->meter[i].AvgVoltage;
    root["c"+String(i+1)+"_meter_current"]      = gState->meter[i].AvgCurrent;
    root["c"+String(i+1)+"_meter_fwdAlertSet"]  = gState->meter[i].fwdAlertSet;
    root["c"+String(i+1)+"_meter_backAlertSet"] = gState->meter[i].backAlertSet;
    root["c"+String(i+1)+"_meter_conf_fwdCLim"] = gConfig->meter[i].fwdCLim;
    root["c"+String(i+1)+"_meter_conf_backCLim"] = gConfig->meter[i].backCLim;
    root["c"+String(i+1)+"_USBInfo_numDev"]     = gState->usbInfo[i].numDev;
    root["c"+String(i+1)+"_USBInfo_Dev1_Name"]  = gState->usbInfo[i].Dev1_Name;
    root["c"+String(i+1)+"_USBInfo_Dev2_Name"]  = gState->usbInfo[i].Dev2_Name;
    root["c"+String(i+1)+"_USBInfo_usbType"]    = gState->usbInfo[i].usbType;
    root["c"+String(i+1)+"_BaseMCU_fault"]      = gState->baseMCUIn[i].fault;
    root["c"+String(i+1)+"_BaseMCU_ilim"]       = gState->baseMCUOut[i].ilim;
    root["c"+String(i+1)+"_BaseMCU_data_en"]    = gState->baseMCUOut[i].data_en;
    root["c"+String(i+1)+"_BaseMCU_pwr_en"]     = gState->baseMCUOut[i].pwr_en;
  }

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

void MasterStateService::getNetworkInfo(){ 
  
  uint8_t tempWifiState = WIFI_OFFLINE ;

  tempWifiState = static_cast<uint8_t>(_skit->getConnectionStatus());
  gState->features.wifiState = tempWifiState;
  gState->features.wifiRSSI = WiFi.RSSI();
  EventSocket *_socket = _skit->getSocket();
  

  if(tempWifiState == AP_NOCLIENT || tempWifiState == AP_CONNECTED){
    gState->features.wifiAPIP = WiFi.softAPIP().toString();
    gState->features.wifiClients = _socket->getConnectedClients();
    wifi_sta_list_t staList;
    if (tempWifiState == AP_CONNECTED) {
      
    }
  }
  else if (tempWifiState == STA_NOCLIENT || tempWifiState == STA_CONNECTED){
    gState->features.wifiIP = WiFi.localIP().toString();
    gState->features.ssid = WiFi.SSID();
    gState->features.wifiClients = _socket->getConnectedClients();   
  }
  else{
    gState->features.wifiIP ="0.0.0.0";
    gState->features.ssid = "";
    gState->features.wifiAPIP = "0.0.0.0";
  }

  //ESP_LOGI("","ssid: %s IP: %s Stat: %u", gState->features.ssid.c_str(), gState->features.wifiIP, gState->features.wifiState);
  
}

void MasterStateService::wifiFallBackCheck(){
  
  WiFiSettingsService *wifiSettingsService = _skit->getWiFiSettingsService();
   
  if(gState->features.wifiState == WIFI_OFFLINE || gState->features.wifiState == AP_NOCLIENT)
    wifiSettingsService->connectToWiFi();

}