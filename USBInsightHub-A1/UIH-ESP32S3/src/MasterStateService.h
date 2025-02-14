#ifndef MasterStateService_h
#define MasterStateService_h


#include <ESP32SvelteKit.h>
#include <EventSocket.h>
#include <HttpEndpoint.h>
#include <EventEndpoint.h>
#include <WebSocketServer.h>
#include "esp_crc.h"

#include "datatypes.h"

#define DEFAULT_POWER_STATE false
#define OFF_STATE "OFF"
#define ON_STATE "ON"

#define MASTER_STATE_ENDPOINT_PATH "/rest/masterState"
#define MASTER_STATE_SOCKET_PATH "/ws/masterState"
#define MASTER_STATE_EVENT "master"

#define FRONTEND_UPDATE_PERIOD 500
#define FALLBACK_TIMER 10000


//Staruct defining data for each USB table
typedef struct 
{
	int startup_counter;
	int startup_conf_timer;
	float meter_voltage;
	float meter_current;
	bool meter_fwdAlertSet;
	bool meter_backAlertSet;
	uint16_t meter_conf_fwdCLim;
	uint16_t meter_conf_backCLim;
	int USBInfo_numDev;
	String USBInfo_Dev1_Name;
	String USBInfo_Dev2_Name;
	int USBInfo_usbType;
	bool BaseMCU_fault;
	uint8_t BaseMCU_ilim;
	bool BaseMCU_data_en;
	bool BaseMCU_pwr_en;
		
} channel_data_t;


class MasterState
{
public:
    bool powerOn;
    bool switchOn;

    channel_data_t chData[3];

    uint8_t features_conf_startUpmode;
    uint8_t features_conf_wifi_enabled;
    uint8_t features_conf_hubMode;
    uint8_t features_conf_filterType;
    uint8_t features_conf_refreshRate;
    bool features_startUpActive;
    bool features_pcConnected;
    float features_vbusVoltage;

    uint8_t screen_conf_rotation;
    uint16_t screen_conf_brightness;

    uint8_t BaseMCU_vext_cc;
    uint8_t BaseMCU_vhost_cc;
    uint8_t BaseMCU_vext_stat;
    uint8_t BaseMCU_vhost_stat;
    bool BaseMCU_pwr_source;
    bool BaseMCU_usb3_mux_out_en;
    bool BaseMCU_usb3_mux_sel_pos;
    uint8_t BaseMCU_base_ver;

    static void read(MasterState &settings, JsonObject &root)
    {
        root["power_on"] = settings.powerOn;
        root["switch_on"] = settings.switchOn;

        //move MasterState data to Stateles json structures
        root["features_conf_startUpmode"]   = settings.features_conf_startUpmode;
        root["features_conf_wifi_enabled"]  = settings.features_conf_wifi_enabled;
        root["features_conf_hubMode"]       = settings.features_conf_hubMode;
        root["features_conf_filterType"]    = settings.features_conf_filterType;
        root["features_conf_refreshRate"]   = settings.features_conf_refreshRate;
        root["features_startUpActive"]      = settings.features_startUpActive;
        root["features_pcConnected"]        = settings.features_pcConnected;
        root["features_vbusVoltage"]        = settings.features_vbusVoltage;
        root["screen_conf_rotation"]        = settings.screen_conf_rotation;
        root["screen_conf_brightness"]      = settings.screen_conf_brightness;
        root["BaseMCU_vext_cc"]             = settings.BaseMCU_vext_cc;
        root["BaseMCU_vhost_cc"]            = settings.BaseMCU_vhost_cc;
        root["BaseMCU_vext_stat"]           = settings.BaseMCU_vext_stat;
        root["BaseMCU_vhost_stat"]          = settings.BaseMCU_vhost_stat;
        root["BaseMCU_pwr_source"]          = settings.BaseMCU_pwr_source;
        root["BaseMCU_usb3_mux_out_en"]     = settings.BaseMCU_usb3_mux_out_en;
        root["BaseMCU_usb3_mux_sel_pos"]    = settings.BaseMCU_usb3_mux_sel_pos;
        root["BaseMCU_base_ver"]            = settings.BaseMCU_base_ver;

        for(int i =0; i<3; i++){
            
            root["c"+String(i+1)+"_startup_counter"]    = settings.chData[i].startup_counter;
            root["c"+String(i+1)+"_startup_conf_timer"] = settings.chData[i].startup_conf_timer;
            root["c"+String(i+1)+"_meter_voltage"]      = settings.chData[i].meter_voltage;
            root["c"+String(i+1)+"_meter_current"]      = settings.chData[i].meter_current;
            root["c"+String(i+1)+"_meter_fwdAlertSet"]  = settings.chData[i].meter_fwdAlertSet;
            root["c"+String(i+1)+"_meter_backAlertSet"] = settings.chData[i].meter_backAlertSet;
            root["c"+String(i+1)+"_meter_conf_fwdCLim"] = settings.chData[i].meter_conf_fwdCLim;
            root["c"+String(i+1)+"_meter_conf_backCLim"] = settings.chData[i].meter_conf_backCLim;
            root["c"+String(i+1)+"_USBInfo_numDev"]     = settings.chData[i].USBInfo_numDev;
            root["c"+String(i+1)+"_USBInfo_Dev1_Name"]  = settings.chData[i].USBInfo_Dev1_Name;
            root["c"+String(i+1)+"_USBInfo_Dev2_Name"]  = settings.chData[i].USBInfo_Dev2_Name;
            root["c"+String(i+1)+"_USBInfo_usbType"]    = settings.chData[i].USBInfo_usbType;
            root["c"+String(i+1)+"_BaseMCU_fault"]      = settings.chData[i].BaseMCU_fault;
            root["c"+String(i+1)+"_BaseMCU_ilim"]       = settings.chData[i].BaseMCU_ilim;
            root["c"+String(i+1)+"_BaseMCU_data_en"]    = settings.chData[i].BaseMCU_data_en;
            root["c"+String(i+1)+"_BaseMCU_pwr_en"]     = settings.chData[i].BaseMCU_pwr_en;
        }

    }

    static StateUpdateResult update(JsonObject &root, MasterState &settings)
    {
        settings.powerOn = root["power_on"] | false;
        settings.switchOn = root["switch_on"] | false;
        
        settings.features_conf_startUpmode  = root["features_conf_startUpmode"].as<uint8_t>();
        settings.features_conf_wifi_enabled = root["features_conf_wifi_enabled"].as<uint8_t>();
        settings.features_conf_hubMode      = root["features_conf_hubMode"].as<uint8_t>();
        settings.features_conf_filterType   = root["features_conf_filterType"].as<uint8_t>();
        settings.features_conf_refreshRate  = root["features_conf_refreshRate"].as<uint8_t>();
        settings.features_startUpActive     = root["features_startUpActive"] | false;
        settings.features_pcConnected       = root["features_pcConnected"] | false;
        settings.features_vbusVoltage       = root["features_vbusVoltage"].as<float>();
        settings.screen_conf_rotation       = root["screen_conf_rotation"].as<uint8_t>();
        settings.screen_conf_brightness     = root["screen_conf_brightness"].as<uint16_t>();
        settings.BaseMCU_vext_cc            = root["BaseMCU_vext_cc"].as<uint8_t>();
        settings.BaseMCU_vhost_cc           = root["BaseMCU_vhost_cc"].as<uint8_t>();
        settings.BaseMCU_vext_stat          = root["BaseMCU_vext_stat"].as<uint8_t>();
        settings.BaseMCU_vhost_stat         = root["BaseMCU_vhost_stat"].as<uint8_t>();
        settings.BaseMCU_pwr_source         = root["BaseMCU_pwr_source"] | false;
        settings.BaseMCU_usb3_mux_out_en    = root["BaseMCU_usb3_mux_out_en"] | false;
        settings.BaseMCU_usb3_mux_sel_pos   = root["BaseMCU_usb3_mux_sel_pos"] | false;
        settings.BaseMCU_base_ver           = root["BaseMCU_base_ver"].as<uint8_t>();                

        for(int i =0; i<3; i++){
            
            settings.chData[i].startup_counter      = root["c"+String(i+1)+"_startup_counter"].as<int>();
            settings.chData[i].startup_conf_timer   = root["c"+String(i+1)+"_startup_conf_timer"].as<int>();
            settings.chData[i].meter_voltage        = root["c"+String(i+1)+"_meter_voltage"].as<float>();
            settings.chData[i].meter_current        = root["c"+String(i+1)+"_meter_current"].as<float>();
            settings.chData[i].meter_fwdAlertSet    = root["c"+String(i+1)+"_meter_fwdAlertSet"] | false;
            settings.chData[i].meter_backAlertSet   = root["c"+String(i+1)+"_meter_backAlertSet" ] | false;
            settings.chData[i].meter_conf_fwdCLim   = root["c"+String(i+1)+"_meter_conf_fwdCLim"].as<uint16_t>();
            settings.chData[i].meter_conf_backCLim  = root["c"+String(i+1)+"_meter_conf_backCLim"].as<uint16_t>();
            settings.chData[i].USBInfo_numDev       = root["c"+String(i+1)+"_USBInfo_numDev"].as<int>();
            settings.chData[i].USBInfo_Dev1_Name    = root["c"+String(i+1)+"_USBInfo_Dev1_Name"].as<String>();
            settings.chData[i].USBInfo_Dev2_Name    = root["c"+String(i+1)+"_USBInfo_Dev2_Name"].as<String>();
            settings.chData[i].USBInfo_usbType      = root["c"+String(i+1)+"_USBInfo_usbType"].as<int>();
            settings.chData[i].BaseMCU_fault        = root["c"+String(i+1)+"_BaseMCU_fault"] | false;
            settings.chData[i].BaseMCU_ilim         = root["c"+String(i+1)+"_BaseMCU_ilim"].as<uint8_t>();
            settings.chData[i].BaseMCU_data_en      = root["c"+String(i+1)+"_BaseMCU_data_en"] | false;
            settings.chData[i].BaseMCU_pwr_en       = root["c"+String(i+1)+"_BaseMCU_pwr_en"] | false;
        }

        return StateUpdateResult::CHANGED;
    }

};

class MasterStateService : public StatefulService<MasterState>
{
public:
    MasterStateService(PsychicHttpServer *server,
                      EventSocket *socket,
                      SecurityManager *securityManager);

    void begin(GlobalState *globalState, GlobalConfig *globalConfig, ESP32SvelteKit *esp32sveltekit);

private:
    HttpEndpoint<MasterState> _httpEndpoint;
    EventEndpoint<MasterState> _eventEndpoint;
    WebSocketServer<MasterState> _webSocketServer;

    GlobalState *gState;
    GlobalConfig *gConfig;
    ESP32SvelteKit *_skit;

    //StaticJsonDocument<2100> masterStateDoc;
    JsonDocument masterStateDoc;
    JsonObject masterStateObj;

    TaskHandle_t taskMSSHandle;
    uint32_t lastHash;

    void onConfigUpdated();
    static void taskMSSImpl(void *pvParameters);
    void taskMSS();

    void copyBackendToGlobal(JsonObject &root);
    void copyGlobalToBackend(JsonObject &root);
    uint32_t calculateJsonHash(JsonObject &root);
    void getNetworkInfo();
    void wifiFallBackCheck();

};

#endif