
#include "Extercomms.h"

//USB Serial and Harware Serial (Debug)
#if ARDUINO_USB_CDC_ON_BOOT
#define HWSerial Serial0
#define USBSerial Serial
#else
#define HWSerial Serial
USBCDC usbSerial;
#endif

static const char* TAG = "Extercoms";

GlobalState *gloState;
GlobalConfig *gloConfig;

bool USBSerialActivity = false;
bool dataReceived = false; 


char rawBuffer[76];
size_t rawBufIndex = 0;
char inputBuffer[MAX_BUFFER_SIZE];   //working array JSON-RPC
size_t bufferIndex = 0;

//Internal functions
void parseDataPC();
static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void taskExterCheckActivity(void *pvParameters);

void onSerialDataReceived();
void processJsonRpcMessage(const char* jsonString);
void sendJsonResponse(int id, JsonVariant result);
void printErr(String err);
int getEnumIndex(const char* name, const char* const* array, int size);


void iniExtercomms(GlobalState* globalState, GlobalConfig* globalConfig){

    gloState = globalState;
    gloConfig = globalConfig;

    //Hardware Serial Ini
    //HWSerial.begin(115200); //Debug Serial
    USB.onEvent(usbEventCallback);
    usbSerial.onEvent(usbEventCallback);

    usbSerial.begin(115200);
    USB.manufacturerName("Aerio");
    USB.productName("InsightHUB Controller");
    USB.begin();

    xTaskCreatePinnedToCore(taskExterCheckActivity, "Extercom check", 4096, NULL, 5, NULL, APP_CORE);

}

//serial loop - check if there has been serial activity to update the pc-connection status icon 
void taskExterCheckActivity(void *pvParameters){
    TickType_t xLastWakeTime = xTaskGetTickCount();
    unsigned long lastPCcom;
    unsigned long now;
    for(;;){
        now= millis();

        if(USBSerialActivity){
          lastPCcom = millis();
          gloState->features.pcConnected = true;
          USBSerialActivity=false;
        }

        if(now-lastPCcom > PC_CONNECTION_TIMEOUT){
          gloState->features.pcConnected = false;
        }

        if(dataReceived){
          processJsonRpcMessage(inputBuffer);  // Process JSON
          dataReceived = false;
        }

        vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(SERIAL_CHECK_PERIOD));
        //vTaskDelay(pdMS_TO_TICKS(SERIAL_CHECK_PERIOD));
    }

}

//void onSerialDataReceived(const uint8_t* data, size_t length){
void onSerialDataReceived(){
  // Process each byte
  for (size_t i = 0; i < rawBufIndex; i++) {
    char c = (char)rawBuffer[i];

    // Prevent buffer overflow
    if (bufferIndex >= MAX_BUFFER_SIZE - 1) {
        bufferIndex = 0;  // Reset buffer on overflow
        Serial.println("{\"jsonrpc\": \"2.0\", \"error\": {\"code\": -32700, \"message\": \"Buffer overflow\"}}");
        return;
    }

    // Store character in buffer
    inputBuffer[bufferIndex++] = c;

    // Check for end of message (`\n`)
    if (c == '\n') {
        inputBuffer[bufferIndex] = '\0';  // Null-terminate string
        dataReceived = true;
        //ESP_LOGI(TAG,"%s",inputBuffer);
        bufferIndex = 0;  // Reset buffer for next message
    }
  }  
}

// Function to process JSON-RPC message
void processJsonRpcMessage(const char* jsonString) {
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, jsonString);

  if (error) {
    String err = "{\"status\": \"error\", \"data\": {\"code\": -32700, \"message\": \"Parse error "+String(error.c_str())+"\"}}";
    printErr(err);    
    return;
  }
  
  if (!doc["action"] || !doc["params"]) {  
    String err = "{\"status\": \"error\", \"data\": {\"code\": -32600, \"message\": \"Invalid request\"}}";
    printErr(err);
    return;
  }

  String action = doc["action"].as<String>();
  JsonDocument responseDoc;
  JsonObject result = responseDoc.to<JsonObject>();  

  if(action == "set"){

    JsonObject params = doc["params"].as<JsonObject>();
    bool pFail = false;
    

    if(params["startUpmode"]){
      int inx = getEnumIndex(params["startUpmode"].as<const char*>(),t_startupMode,ARR_SIZE(t_startupMode));
      inx != -1 ? gloConfig->features.startUpmode = inx : result["startUpmode"] = "fail";
    }
    if(params["wifi_enabled"]) {
      int inx = getEnumIndex(params["wifi_enabled"].as<const char*>(),t_bool,ARR_SIZE(t_bool));
      inx != -1 ? gloConfig->features.wifi_enabled = inx : result["wifi_enabled"] = "fail";
    }
    
    if(params["hubMode"]){
      int inx = getEnumIndex(params["hubMode"].as<const char*>(),t_hubMode,ARR_SIZE(t_hubMode));
      inx != -1 ? gloConfig->features.hubMode = inx : result["hubMode"] = "fail";
    }
    if(params["filterType"]){
      int inx = getEnumIndex(params["filterType"].as<const char*>(),t_filterType,ARR_SIZE(t_filterType));
      inx != -1 ? gloConfig->features.filterType = inx : result["filterType"] = "fail";
    }
    if(params["refreshRate"]){
      int inx = getEnumIndex(params["refreshRate"].as<const char*>(),t_refreshRate,ARR_SIZE(t_refreshRate));
      inx != -1 ? gloConfig->features.refreshRate = inx : result["refreshRate"] = "fail";
    }        
    if(params["rotation"]){
      int inx = getEnumIndex(params["rotation"].as<const char*>(),t_rotation,ARR_SIZE(t_rotation));
      if(inx != -1) {
        gloConfig->screen[0].rotation = inx;
        gloConfig->screen[1].rotation = inx;
        gloConfig->screen[2].rotation = inx;
      }
      else
        result["rotation"] = "fail";
    }   
    if(params["brightness"]){
      uint8_t inx = params["brightness"].as<unsigned int>();
      if(inx >= 10 && inx <= 100) {
        gloConfig->screen[0].brightness = inx;
        gloConfig->screen[1].brightness = inx;
        gloConfig->screen[2].brightness = inx;
      }
      else
        result["brightness"] = "fail";
    } 
    
    if(params["ledState"]){
      int inx = getEnumIndex(params["ledState"].as<const char*>(),t_bool,ARR_SIZE(t_bool));
      inx != -1 ? gloState->system.ledState = inx : result["ledState"] = "fail";        
    }

    for(int i = 0; i<3; i++){

      if(params["CH"+String(i+1)]["powerEn"]){
        int inx = getEnumIndex(params["CH"+String(i+1)]["powerEn"].as<const char*>(),t_bool,ARR_SIZE(t_bool));
        inx != -1 ? gloState->baseMCUOut[i].pwr_en = inx : result["CH"+String(i+1)]["powerEn"] = "fail";        
      }
      if(params["CH"+String(i+1)]["dataEn"]){
        int inx = getEnumIndex(params["CH"+String(i+1)]["dataEn"].as<const char*>(),t_bool,ARR_SIZE(t_bool));
        inx != -1 ? gloState->baseMCUOut[i].data_en = inx :  result["CH"+String(i+1)]["dataEn"] = "fail";        
      }      
      if(params["CH"+String(i+1)]["startup_tmr"]){
        uint8_t inx = params["CH"+String(i+1)]["startup_tmr"].as<unsigned int>();
        (inx >= 1 && inx <= 100) ? gloConfig->startup[i].startup_timer = inx :  result["CH"+String(i+1)]["startup_tmr"] = "out of range";
      }
      if(params["CH"+String(i+1)]["fwdLimit"]){
        uint16_t inx = params["CH"+String(i+1)]["fwdLimit"].as<unsigned int>();
        (inx >= 100 && inx <= 2000) ? gloConfig->meter[i].fwdCLim = inx : result["CH"+String(i+1)]["fwdLimit"] = "out of range";
      }
      if(params["CH"+String(i+1)]["backLimit"]){
        uint16_t inx = params["CH"+String(i+1)]["backLimit"].as<unsigned int>();
        (inx >= 1 && inx <= 200) ? gloConfig->meter[i].backCLim = inx : result["CH"+String(i+1)]["backLimit"] = "out of range";
      }

      if(params["CH"+String(i+1)]["fwdAlert"]){
        int inx = getEnumIndex(params["CH"+String(i+1)]["fwdAlert"].as<const char*>(),t_bool,ARR_SIZE(t_bool));
        inx != -1 ? gloState->meter[i].fwdAlertSet = inx : result["CH"+String(i+1)]["fwdAlert"] = "fail";        
      }
      if(params["CH"+String(i+1)]["backAlert"]){
        int inx = getEnumIndex(params["CH"+String(i+1)]["backAlert"].as<const char*>(),t_bool,ARR_SIZE(t_bool));
        inx != -1 ? gloState->meter[i].backAlertSet = inx : result["CH"+String(i+1)]["backAlert"] = "fail";        
      }
      if(params["CH"+String(i+1)]["shortAlert"]){
        int inx = getEnumIndex(params["CH"+String(i+1)]["shortAlert"].as<const char*>(),t_bool,ARR_SIZE(t_bool));
        inx != -1 ? gloState->baseMCUIn[i].fault = inx : result["CH"+String(i+1)]["shortAlert"] = "fail";        
      }          

      if(params["CH"+String(i+1)]["numDev"]){
        uint8_t inx = params["CH"+String(i+1)]["numDev"].as<unsigned int>();
        (inx >= 0 && inx <= 10) ? gloState->usbInfo[i].numDev = inx : result["CH"+String(i+1)]["numDev"] = "out of range";
      }
      if(params["CH"+String(i+1)]["Dev1_name"]){
        gloState->usbInfo[i].Dev1_Name = params["CH"+String(i+1)]["Dev1_name"].as<String>();        
      }
      if(params["CH"+String(i+1)]["Dev2_name"]){
        gloState->usbInfo[i].Dev2_Name = params["CH"+String(i+1)]["Dev2_name"].as<String>();        
      }
      if(params["CH"+String(i+1)]["usbType"]){
        uint8_t inx = params["CH"+String(i+1)]["usbType"].as<unsigned int>();
        (inx >= 0 && inx <= 3) ? gloState->usbInfo[i].usbType = inx : result["CH"+String(i+1)]["usbType"] = "out of range";
      }

    }

    result["valid"] = String(params.size()-result.size()) + " of " + String(params.size());

    sendJsonResponse(0, result);
    
  }  

  if(action == "get") {  
    JsonArray params = doc["params"].as<JsonArray>();
    JsonDocument responseDoc;
    JsonObject result = responseDoc.to<JsonObject>();
    

    for (JsonVariant v : params) {
      String pName = v.as<String>();
      
      bool all, conf, state;
      
      pName == "all" ? all = true : all = false;
      pName == "config" ? conf = true : conf = false;
      pName == "state" ? state = true: state = false;
      
      if(pName == "startUpmode"   || all || conf)  
        result["startUpmode"]   = t_startupMode[gloConfig->features.startUpmode];
      if(pName == "wifi_enabled"  || all || conf) 
        result["wifi_enabled"]  = gloConfig->features.wifi_enabled;
      if(pName == "hubMode"       || all || conf)      
        result["hubMode"]       = t_hubMode[gloConfig->features.hubMode];
      if(pName == "filterType"    || all || conf)   
        result["filterType"]    = t_filterType[gloConfig->features.filterType];
      if(pName == "refreshRate"   || all || conf)  
        result["refreshRate"]   = t_refreshRate[gloConfig->features.refreshRate];
      if(pName == "rotation"      || all || conf)     
        result["rotation"]      = t_rotation[gloConfig->screen[0].rotation];
      if(pName == "brightness"    || all || conf)   
        result["brightness"]    = gloConfig->screen[0].brightness;
      
      if(pName == "startUpActive" || all || state)
        result["startUpActive"] = gloState->features.startUpActive;
      if(pName == "pcConnected"   || all || state)  
        result["pcConnected"]   = gloState->features.pcConnected;
      if(pName == "vbus"   || all || state)  
        result["vbus"]   = String(gloState->features.vbus,0);
      if(pName == "vext_cc"       || all || state)      
        result["vext_cc"]       = t_vx_cc[gloState->baseMCUExtra.vext_cc];
      if(pName == "vhost_cc"      || all || state)     
        result["vhost_cc"]      = t_vx_cc[gloState->baseMCUExtra.vhost_cc];
      if(pName == "vext_stat"     || all || state)    
        result["vext_stat"]     = t_vx_stat[gloState->baseMCUExtra.vext_stat];
      if(pName == "vhost_stat"    || all || state)   
        result["vhost_stat"]    = t_vx_stat[gloState->baseMCUExtra.vhost_stat];
      if(pName == "pwr_source"    || all || state)   
        gloState->baseMCUExtra.pwr_source ? result["pwr_source"] = "vext": result["pwr_source"] = "vhost";
      if(pName == "usb3_mux_out_en" || all || state) 
        result["usb3_mux_out_en"] = gloState->baseMCUExtra.usb3_mux_out_en;
      if(pName == "usb3_mux_sel_pos" || all || state) 
        gloState->baseMCUExtra.usb3_mux_sel_pos ? result["usb3_mux_sel_pos"] = "1" : result["usb3_mux_sel_pos"] = "0";
      if(pName == "base_ver"      || all || state)     
        result["base_ver"]      = gloState->baseMCUExtra.base_ver;
      if(pName == "esp32_ver"     || all || state)
        result["cpu_ver"]       = APP_VERSION;
      if(pName == "mac"           || all || state)
        result["mac"]           = gloState->system.wifiMAC; 
      
      //production specific getters
    
      if(pName == "cpu_freq"    || all || state)
        result["cpu_freq"]      = String(ESP.getCpuFreqMHz());
      if(pName == "ledState"    || all || state)
        result["ledState"]      = gloState->system.ledState;     
      if(pName == "firstStart"    || all || state){
        result["firstStart"]    = gloState->system.firstStart;
        gloState->system.firstStart = false;
      }
      if(pName == "menuIsActive"    || all || state)
        result["menuIsActive"]      = gloState->system.menuIsActive;
                 
      for(int i = 0; i<3; i++){
        if (pName == "CH"+String(i+1) || pName == "CH"+String(i+1)+"_all"){
          result["CH"+String(i+1)]["voltage"]     = String(gloState->meter[i].AvgVoltage,1);
          result["CH"+String(i+1)]["current"]     = String(gloState->meter[i].AvgCurrent,1);
          result["CH"+String(i+1)]["fwdAlert"]    = gloState->meter[i].fwdAlertSet;
          result["CH"+String(i+1)]["backAlert"]   = gloState->meter[i].backAlertSet;
          result["CH"+String(i+1)]["shortAlert"]  = gloState->baseMCUIn[i].fault;          
          result["CH"+String(i+1)]["dataEn"]      = gloState->baseMCUOut[i].data_en;
          result["CH"+String(i+1)]["powerEn"]     = gloState->baseMCUOut[i].pwr_en;
        }
        if(pName == "CH"+String(i+1)+"_all"){
          result["CH"+String(i+1)]["ilim"]        = gloState->baseMCUOut[i].ilim;
          result["CH"+String(i+1)]["startup_cnt"] = gloState->startup[i].startup_cnt;
          result["CH"+String(i+1)]["startup_tmr"] = gloConfig->startup[i].startup_timer;
          result["CH"+String(i+1)]["fwdLimit"]    = gloConfig->meter[i].fwdCLim;
          result["CH"+String(i+1)]["backLimit"]   = gloConfig->meter[i].backCLim;
          result["CH"+String(i+1)]["numDev"]      = gloState->usbInfo[i].numDev;
          result["CH"+String(i+1)]["Dev1_name"]   = gloState->usbInfo[i].Dev1_Name;
          result["CH"+String(i+1)]["Dev2_name"]   = gloState->usbInfo[i].Dev2_Name;
          result["CH"+String(i+1)]["usbType"]     = gloState->usbInfo[i].usbType;
        } 

      }

    }

    sendJsonResponse(0, result);
  }
}

// Send JSON-RPC response
void sendJsonResponse(int id, JsonVariant result) {
  JsonDocument doc;
  doc["status"] = "ok";
  doc["data"] = result;

  String response;
  serializeJson(doc, response);
  usbSerial.println(response);
  usbSerial.flush();
}

void printErr(String err){
  
  usbSerial.println(err);
  usbSerial.flush();
  ESP_LOGI(TAG," %s", err.c_str());
}

int getEnumIndex(const char* name, const char* const* array, int size) {
  for (int i = 0; i < size; ++i) {
      //ESP_LOGI(TAG,"size: %u, name: %s, array: %s, i: %u", size,name,array[i],i);
      if (strcmp(array[i], name) == 0) {
          return i;
      }
  }
  return -1; // Return -1 if not found
}

//Handlers for TinyUSB events when working in CDC mode

static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
  if(event_base == ARDUINO_USB_EVENTS)
  {
    arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_STARTED_EVENT:
        //HWSerial.println("USB PLUGGED");
        ESP_LOGI(TAG,"USB PLUGGED");
        break;
      case ARDUINO_USB_STOPPED_EVENT:
        //HWSerial.println("USB UNPLUGGED");
        ESP_LOGI(TAG,"USB UNPLUGGED");
        break;
      case ARDUINO_USB_SUSPEND_EVENT:
        //HWSerial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
        ESP_LOGI(TAG,"USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
        break;
      case ARDUINO_USB_RESUME_EVENT:
        //HWSerial.println("USB RESUMED");
        ESP_LOGI(TAG,"USB RESUMED");
        break;
      
      default:
        break;
    }
  } else if(event_base == ARDUINO_USB_CDC_EVENTS)
  {
    arduino_usb_cdc_event_data_t * data = (arduino_usb_cdc_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_CDC_CONNECTED_EVENT:        
        ESP_LOGI(TAG,"CDC CONNECTED");
        break;
      case ARDUINO_USB_CDC_DISCONNECTED_EVENT: 
        ESP_LOGI(TAG,"CDC DISCONNECTED");
        break;
      case ARDUINO_USB_CDC_LINE_STATE_EVENT:
        ESP_LOGI(TAG,"CDC LINE STATE: dtr: %u, rts: %u\n", data->line_state.dtr, data->line_state.rts);
        break;
      case ARDUINO_USB_CDC_LINE_CODING_EVENT:
        ESP_LOGI(TAG,"CDC LINE CODING: bit_rate: %u, data_bits: %u, stop_bits: %u, parity: %u\n", data->line_coding.bit_rate, data->line_coding.data_bits, data->line_coding.stop_bits, data->line_coding.parity);
        break;
      case ARDUINO_USB_CDC_RX_EVENT:
        //ESP_LOGV(TAG,"CDC RX [%u]:", data->rx.len);
        {
            uint8_t buf[data->rx.len];

            rawBufIndex = usbSerial.read(rawBuffer,data->rx.len);
            onSerialDataReceived();

            USBSerialActivity=true;
            gloState->features.pcConnected = true;
            
        }
        break;
      case ARDUINO_USB_CDC_RX_OVERFLOW_EVENT:
        ESP_LOGW(TAG,"CDC RX Overflow of %d bytes", data->rx_overflow.dropped_bytes);
        break;
     
      default:
        break;
    }
  }
}
