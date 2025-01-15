
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

bool USBSerialActivity = false;

//Port values from pc strings
char *pcstrings[13]={"PC","0","-","-","0","0","-","-","0","0","-","-","0"}; //array of pointers to each value
/*{"PC",<CH1_NUM_OF_DEVICES>,<CH1_USB_DEV_1>,<CH1_USB_DEV_2>,<CH1_USB_TYPE>
        <CH2_NUM_OF_DEVICES>,<CH2_USB_DEV_1>,<CH2_USB_DEV_2>,<CH2_USB_TYPE>
        <CH3_NUM_OF_DEVICES>,<CH3_USB_DEV_1>,<CH3_USB_DEV_2>,<CH3_USB_TYPE>
}*/
char *pcptr = NULL;
char pcsbuffer[60]; //working array
String tempString;

//Internal functions
void parseDataPC();
static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void taskExterCheckActivity(void *pvParameters);


void iniExtercomms(GlobalState* globalState){

    gloState = globalState;

    //Hardware Serial Ini
    //HWSerial.begin(115200); //Debug Serial
    USB.onEvent(usbEventCallback);
    usbSerial.onEvent(usbEventCallback);

    usbSerial.begin();
    USB.manufacturerName("Aerio");
    USB.productName("InsightHUB Controller");
    USB.begin();

    xTaskCreatePinnedToCore(taskExterCheckActivity, "Extercom check", 1024, NULL, 5, NULL, APP_CORE);

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
        vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(SERIAL_CHECK_PERIOD));
        //vTaskDelay(pdMS_TO_TICKS(SERIAL_CHECK_PERIOD));
    }

}

void parseDataPC() {
  //ref: https://forum.arduino.cc/t/separating-a-comma-delimited-string/682868/6

  byte index = 0;
  pcptr = strtok(pcsbuffer,",");
  while(pcptr != NULL){        
    pcstrings[index] = pcptr;
    index++;
    pcptr = strtok(NULL, ",");
  }

  if(String(pcstrings[0])=="PC"){
    for(int i=0;i<3;i++){       
        gloState->usbInfo[i].numDev     = String(pcstrings[4*i+1]).toInt();
        gloState->usbInfo[i].Dev1_Name  = String(pcstrings[4*i+2]);
        gloState->usbInfo[i].Dev2_Name  = String(pcstrings[4*i+3]);
        gloState->usbInfo[i].usbType    = String(pcstrings[4*i+4]).toInt();
    } 
  }

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
            size_t len = usbSerial.read(buf, data->rx.len);

            //pcsbuffer=(char)(buf);
            memcpy(pcsbuffer,buf,len);
            //tempString.toCharArray(pcsbuffer, tempString.length()+1);
            parseDataPC();
            USBSerialActivity=true;
            gloState->features.pcConnected = true;
            //HWSerial.write(buf, len);
        }
        //HWSerial.println();
        break;
       case ARDUINO_USB_CDC_RX_OVERFLOW_EVENT:
        ESP_LOGW(TAG,"CDC RX Overflow of %d bytes", data->rx_overflow.dropped_bytes);
        break;
     
      default:
        break;
    }
  }
}
