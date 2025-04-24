/**
 *   USB Insight Hub
 *
 *   A USB supercharged interfacing tool for developers & tech enthusiasts wrapped 
 *   around ESP32 SvelteKit framework.
 *   https://github.com/Aeriosolutions/USB-Insight-HUB-Software
 *
 *   Copyright (C) 2024 - 2025 Aeriosolutions
 *   Copyright (C) 2024 - 2025 JoDaSa

 * MIT License. Check full description on LICENSE file.
 **/

  //Logic for the menu view. Handling of states an dprints when navigating the interface

#include "MenuView.h"

GlobalState *gSte;
GlobalConfig *gCon;
Screen *iScr;

static const char* TAG = "MenuView";

uint8_t mLevel = 0; //menu level
uint8_t mIndex = 0; //menu Index (current pointed element)
bool treeEnd = false;
unsigned long lastButtonActivity;

Menu* currentMenu;
std::stack<Menu*> menuStack;

Menu CH1Config = {
    TYPE_ROOT,
    "CH1 Config.",
    {
        {TYPE_RANGE,"Over Current",   {},{"100","2000","100"},"mA",{H_OC}},
        {TYPE_RANGE,"Back Current",{},{"1","200","10"},"mA",{H_RC}},
        {TYPE_RANGE,"Startup Timer",  {},{"1","100","5"},"s",{H_CHSTUP}}
    },{"0"},"",{H_CHxCONF}
};

Menu CH2Config = {
    TYPE_ROOT,
    "CH2 Config.",
    {
        {TYPE_RANGE,"Over Current",   {},{"100","2000","100"},"mA",{H_OC}},
        {TYPE_RANGE,"Back Current",{},{"1","200","10"},"mA",{H_RC}},
        {TYPE_RANGE,"Startup Timer",  {},{"1","100","5"},"s",{H_CHSTUP}}
    },{"1"},"",{H_CHxCONF}
};

Menu CH3Config = {
    TYPE_ROOT,
    "CH3 Config.",
    {
        {TYPE_RANGE,"Over Current",   {},{"100","2000","100"},"mA",{H_OC}},
        {TYPE_RANGE,"Back Current",{},{"1","200","10"},"mA",{H_RC}},
        {TYPE_RANGE,"Startup Timer",  {},{"1","100","5"},"s",{H_CHSTUP}}
    },{"2"},"",{H_CHxCONF}
};

Menu generalConfig = {
    TYPE_ROOT,
    "Global Config.",
    {
        {TYPE_ROOT, "Wi-Fi",{
            {TYPE_INFO,"WiFi Info",{},{},"",{H_NONE}},
            {TYPE_SELECT,"WiFi Recovery",{},{"No Action", "Recovery"},"",{H_WIREC,H_WIRExNA,H_WIRECREC}},
            {TYPE_SELECT,"WiFi Reset",{},{"No Action", "Reset"},"",{H_WIRES,H_WIRExNA,H_WIRESRES}},
            {TYPE_SELECT,"WiFi Enable",{},{"Disable", "Enable"},"",{H_WIEN,H_WIENNO,H_WIENYES}}
        },{},"",{H_WIGEN}},
        {TYPE_SELECT, "Startup Mode",{},{"Persistence", "On at startup", "Off at startup", "Timed"},"",
        {H_GLSTUP,H_GLSTUPPER,H_GLSTUPON,H_GLSTUPOFF,H_GLSTUPTMR}},
        {TYPE_ROOT, "Meter",{
            {TYPE_SELECT,"Refresh Rate",{},{"0.5s","1.0s"},"",{H_METREF,H_METREF,H_METREF}},
            {TYPE_SELECT,"Filter Type",{},{"Moving Avg.","Median"},"",{H_METFILT,H_METFILTMA,H_METFILTMED}}
        },{},"",{H_METER}},        
        {TYPE_ROOT, "Screen",{
            {TYPE_SELECT,"Rotation",{},{"0","90","180","270"},"DEG",{H_SCRROT,H_NONE,H_NONE,H_NONE,H_NONE}},
            {TYPE_RANGE,"Brightness",{},{"50","1000","50"},"%",{H_SCRBRI}}
        },{},"",{H_SCREEN}},
        {TYPE_SELECT, "HUB Mode",{},{"USB2 & USB3", "USB2 Only", "USB3 Only"},"",
        {H_USBTYP,H_USBTYP23,H_USBTYP2,H_USBTYP3}},
        {TYPE_SELECT,"Restore Default",{},{"No Action", "Restore Def"},"",{H_DEF,H_DEFxNA,H_DEFRES}}        
    },
    {"0"},"", {H_GLOCONF}
};

Menu mainMenu = {
    TYPE_ROOT,
    "Configurations",
    {
        CH1Config,
        CH2Config,
        CH3Config,
        generalConfig
    },
    {"0"},"", {}
};

uint16_t  getParamValue(String parameter,String channel="0");
void setParamValue(String param, uint16_t value, String channel="0");

void rootLayout(Menu* root, int index);
void selectLayout(Menu* root, int index);
void rangeLayout(Menu* root, String channel="0");
void screenWiFiInfoRender(void);
void resetAutoTimer();

void taskMenuViewLoop(void *pvParameters);


void menuViewStart(GlobalState* globalState, GlobalConfig* globalConfig, Screen *screen){

    gSte = globalState;
    gCon = globalConfig;
    iScr = screen;

    xTaskCreatePinnedToCore(taskMenuViewLoop, "MenuScreen", 5120, NULL, 4, NULL,APP_CORE);
}

void taskMenuViewLoop(void *pvParameters){
    ESP_LOGI(TAG,"Loop on Core %u",xPortGetCoreID());
    lastButtonActivity = millis();
    if(xSemaphoreTake(screen_Semaphore,pdMS_TO_TICKS(60) ) == pdTRUE)
    {   
        //place main menu
        mIndex=0;
        treeEnd = false;        
        currentMenu=&mainMenu;
        String ch = "0";
        uint16_t sel;
        uint16_t step;
        uint16_t rmin;
        uint16_t rmax;
        uint8_t loop = 0;
        iScr->screenSetBackLight(0);
        rootLayout(currentMenu,mIndex);
        iScr->screenSetBackLight(800);
        gSte->system.menuIsActive = true;

        for(;;){

            if(btnShortCheck(0)){                
                
                //Exit soft button
                if(currentMenu->name == "Configurations"){
                    ESP_LOGI(TAG,"Exit button press");
                    xSemaphoreGive(screen_Semaphore);
                    btnClearAll();
                    defaultViewStart();
                    ESP_LOGI(TAG,"Delete Task Loop");
                    gSte->system.menuIsActive = false;
                    vTaskDelete(NULL);                    
                }                
                //Back soft button
                if(!menuStack.empty()){
                    currentMenu = menuStack.top();
                    menuStack.pop();
                    mIndex = 0;
                    treeEnd = false;
                    if(!currentMenu->params.empty()) ch = currentMenu->params[0];
                    rootLayout(currentMenu,mIndex);
                }

                resetAutoTimer();
            }
            if(btnShortCheck(1)){
                //Move
                if(currentMenu->menuType == TYPE_ROOT)
                {
                    mIndex = (mIndex+1) % currentMenu->submenus.size();
                    rootLayout(currentMenu,mIndex);
                }
                if(currentMenu->menuType == TYPE_SELECT)
                {
                    mIndex = (mIndex+1) % currentMenu->params.size();
                    selectLayout(currentMenu,mIndex);
                }
                if(currentMenu->menuType == TYPE_RANGE){
                    
                    if(treeEnd){
                        sel = getParamValue(currentMenu->name,ch);                        
                        rmin = (uint16_t)(currentMenu->params[0].toInt());                        
                        step = (uint16_t)(currentMenu->params[2].toInt());
                        ESP_LOGI(TAG,"%s, %u, %u, %u",ch,sel,rmin,step);

                        if(currentMenu->name == "Startup Timer" && sel == 5){
                            sel = 1;
                            setParamValue(currentMenu->name,sel,ch);
                        } 
                        if(sel - step >= rmin){
                            //round to the closes lower value factor of step                            
                            sel%step == 0 ? sel = sel - step : sel = (sel/step)*step;                            
                            setParamValue(currentMenu->name,sel,ch);
                        }                         
                    }                     

                    rangeLayout(currentMenu,ch);                     
                }

                resetAutoTimer();                                
            }
            if(btnShortCheck(2)){
                //Select
                
                //if(!currentMenu->submenus.empty()){
                if(!treeEnd && currentMenu->menuType != TYPE_INFO){
                    menuStack.push(currentMenu);
                    if(!currentMenu->params.empty()) ch=currentMenu->params[0];
                    currentMenu=&currentMenu->submenus[mIndex];                    
                    mIndex=0;
                }                    
                
                if(currentMenu->menuType == TYPE_ROOT)
                {                       
                    rootLayout(currentMenu,mIndex);                    
                }
                if(currentMenu->menuType == TYPE_SELECT)
                {                                           
                    if(treeEnd){
                        setParamValue(currentMenu->name,(uint16_t)(mIndex));
                    }
                    selectLayout(currentMenu,mIndex);
                    treeEnd = true;                    
                }
                if(currentMenu->menuType == TYPE_RANGE){
                    
                    if(treeEnd){
                        sel = getParamValue(currentMenu->name,ch);                        
                        rmax = (uint16_t)(currentMenu->params[1].toInt());
                        step = (uint16_t)(currentMenu->params[2].toInt());
                        //ESP_LOGI(TAG,"%s, %u, %u, %u",ch,sel,rmax,step);
                        if(sel + step <= rmax) {
                            if(currentMenu->name == "Startup Timer" && sel < 5) 
                                sel = 5;
                            else
                                //round to the closes higher value factor of step 
                                sel = (sel/step)*step + step;
                            setParamValue(currentMenu->name,sel,ch);    
                        }                        
                    }                   

                    rangeLayout(currentMenu,ch);
                    treeEnd = true;                      
                }

                resetAutoTimer();

            }
            if(btnShortCheck(3)){
                ESP_LOGI(TAG,"Setup key");
                resetAutoTimer();
            }

            if (btnLongCheck(3) || (millis() - lastButtonActivity > AUTO_EXIT_TIMEOUT) ){
            
                ESP_LOGI(TAG,"Setup button long press");
                xSemaphoreGive(screen_Semaphore);
                btnClearAll();
                defaultViewStart();
                ESP_LOGI(TAG,"Delete Task Loop");
                gSte->system.menuIsActive = false;
                vTaskDelete(NULL);
            }

            //refresh wifi info every 1s
            if(loop == (1000/MENU_VIEW_PERIOD -1) ){
                if(currentMenu->menuType == TYPE_ROOT)
                    if(currentMenu->submenus[mIndex].name == "WiFi Info"){
                        screenWiFiInfoRender();
                        resetAutoTimer(); //if in this view, disable auto exit
                    }                
                loop = 0;     
            }
            else{
                loop++;
            }

            //update displays if global configuration from backend changes
            if(gSte->system.congigChangedToMenu){
                if(currentMenu->menuType == TYPE_RANGE){
                    rangeLayout(currentMenu,ch);
                }
                if(currentMenu->menuType == TYPE_SELECT){
                    mIndex = getParamValue(currentMenu->name,ch);
                    selectLayout(currentMenu,mIndex);
                }
                gSte->system.congigChangedToMenu = false;
            }

            vTaskDelay(pdMS_TO_TICKS(MENU_VIEW_PERIOD));
        }    
    }
    else {
        ESP_LOGE(TAG, "Screen resource taken, could not initialize");
        defaultViewStart();
        ESP_LOGI(TAG,"Delete Task Loop");
        gSte->system.menuIsActive = false;
        vTaskDelete(NULL);    
    }

}

void rootLayout(Menu* root, int index){
    
    ESP_LOGI("","-------------");
    ESP_LOGI("","%s",root->name.c_str());  
    //ESP_LOGI("","Type: %u",root->menuType);
    screenMenuIntroRender(root,iScr,String(gSte->baseMCUExtra.base_ver));  
    ESP_LOGI("","-------------");
    for(int i=0; i< root->submenus.size(); i++){
        if(index == i)
            ESP_LOGI("","[%s]",root->submenus[i].name.c_str());
        else  
            ESP_LOGI(""," %s",root->submenus[i].name.c_str());
    }
    screenMenuListRender(root,iScr,index,-1);

    ESP_LOGI("","-------------");
    ESP_LOGI("","Help Index: %u", root->submenus[index].helpReference[0]);

    if(root->submenus[index].name != "WiFi Info"){
        screenMenuInfoRender(root,iScr,0,index);
    }
}

void selectLayout(Menu* root, int index){

    ESP_LOGI("","-------------");
    ESP_LOGI("","%s",root->name.c_str());
    screenMenuIntroRender(root,iScr);     
    ESP_LOGI("","-------------");
    if(root->menuType == TYPE_SELECT){

        int sel = (int)(getParamValue(root->name));
        String strm ="";
        for(int i=0; i< root->params.size(); i++){            
            strm=root->params[i];
            if(sel == i) strm = strm + "*";
            if(index == i)                
                ESP_LOGI("","[%s]", strm.c_str());
            else  
                ESP_LOGI(""," %s" , strm.c_str());
        }
        screenMenuListRender(root,iScr,index,sel);
    }

    ESP_LOGI("","-------------");
    ESP_LOGI("","Help Index: %u", root->helpReference[0]);

    screenMenuInfoRender(root,iScr,0,index);
    
}

void rangeLayout(Menu* root, String channel){
    
    uint16_t sel = getParamValue(root->name, channel);
    ESP_LOGI("","-------------");
    ESP_LOGI("","%s",root->name.c_str());
    screenMenuIntroRender(root,iScr,channel);     
    ESP_LOGI("","-------------");
    ESP_LOGI("","<< %u >>",sel);
    screenMenuRangeRender(sel,root->paramUnits,iScr);
    ESP_LOGI("","-------------");
    ESP_LOGI("","Help Index: %u", root->helpReference[0]);
    screenMenuInfoRender(root,iScr,sel);
}


void screenWiFiInfoRender(void){
    digitalWrite(iScr->dProp[2].cs_pin,LOW);
    iScr->tft.setRotation(ROT_180_DEG);
    iScr->img.fillScreen(TFT_BLACK); 
    //header
    iScr->img.fillRoundRect(5,17,230,6,1,TFT_LIGHTGREY);

    iScr->img.loadFont(SMALLFONT);
    iScr->img.setTextFont(2);
    iScr->img.setTextColor(TFT_WHITE);
    String mode;
    String status;
    switch(gSte->features.wifiState){
        case WIFI_OFFLINE:             
            iScr->img.drawString("Mode: Offline",6,45,4);
            iScr->img.drawString("Stat: -",6,85,4);
            break;
        case AP_NOCLIENT: 
        case AP_CONNECTED: 
            iScr->img.drawString("Mode: AP",0,40,4);
            if(gSte->features.wifiState == AP_NOCLIENT){
                iScr->img.drawString("Stat: No Clients",0,80,4);
            }
            else{
                iScr->img.drawString("Stat: " +String(gSte->features.wifiClients)+ String(" Clients"),0,80,4);
            }
            iScr->img.drawString("AP Name ("+String(gSte->features.wifiRSSI)+String(" dB):"),0,120,4);
            iScr->img.setTextColor(TFT_CYAN);
            iScr->img.drawString(gSte->system.APSSID,0,145,4);
            iScr->img.setTextColor(TFT_WHITE);
            iScr->img.drawString("IP: "+ gSte->features.wifiAPIP,0,205,4);
            break;
        case STA_NOCLIENT:            
        case STA_CONNECTED:    
            iScr->img.drawString("Mode: Station",0,40,4);
            if(gSte->features.wifiState == STA_NOCLIENT){
                iScr->img.drawString("Stat: No Clients",0,80,4);
            }
            else{
                iScr->img.drawString("Stat: " +String(gSte->features.wifiClients)+ String(" Clients"),0,80,4);
            }            
            iScr->img.drawString("SSID ("+String(gSte->features.wifiRSSI)+String(" dB):"),0,120,4);
            iScr->img.setTextColor(TFT_CYAN);
            iScr->img.drawString(gSte->features.ssid,0,145,4);
            iScr->img.setTextColor(TFT_WHITE);            
            iScr->img.drawString("IP: "+ gSte->features.wifiIP,0,205,4);             
            break;
        case WIFI_OFF: 
            iScr->img.drawString("Mode: Wi-Fi Off",6,45,4);
            iScr->img.drawString("Stat: -",6,85,4);
            break;
        default:
            break;
    }
   
    iScr->img.unloadFont();
    iScr->img.pushSprite(0,0);
    digitalWrite(iScr->dProp[2].cs_pin, HIGH);    

}

uint16_t getParamValue(String param, String channel){

    if(param =="Over Current") {    
        return ((uint16_t)(gCon->meter[channel.toInt()].fwdCLim));
    }
    if(param =="Back Current") {    
        return ((uint16_t)(gCon->meter[channel.toInt()].backCLim));
    }
    if(param =="Startup Timer") {    
        return ((uint16_t)(gCon->startup[channel.toInt()].startup_timer));
    }
    if(param == "WiFi Recovery") {    
        return ((uint16_t)(gSte->features.wifiRecovery));
    }
    if(param == "WiFi Reset") {    
        return ((uint16_t)(gSte->features.wifiReset));
    }    
    if(param =="WiFi Enable") {    
        return ((uint16_t)(gCon->features.wifi_enabled));
    }
    if(param =="Startup Mode") {    
        return ((uint16_t)(gCon->features.startUpmode));
    }
    if(param =="Refresh Rate") {    
        return ((uint16_t)(gCon->features.refreshRate));
    }
    if(param =="Filter Type") {    
        return ((uint16_t)(gCon->features.filterType));
    }
    if(param =="Rotation") {    
        return ((uint16_t)(gCon->screen[channel.toInt()].rotation));
    }
    if(param =="Brightness") {    
        return ((uint16_t)(gCon->screen[channel.toInt()].brightness));
    }
    if(param =="HUB Mode") {    
        return ((uint16_t)(gCon->features.hubMode));
    }
    if(param =="Restore Default") {            
        return ((uint16_t)(gSte->system.resetToDefault));
    }
    return (0);   
}

void setParamValue(String param, uint16_t value, String channel){
    gSte->system.configChangedFromMenu = true;
    if(param =="Over Current") {    
        gCon->meter[channel.toInt()].fwdCLim = value;
        return;
    }
    if(param =="Back Current") {    
        gCon->meter[channel.toInt()].backCLim = value;
        return;
    }
    if(param =="Startup Timer") {    
        gCon->startup[channel.toInt()].startup_timer = (int)(value);
        return;
    }
    if(param =="WiFi Recovery") {    
        gSte->features.wifiRecovery = (uint8_t)(value);
        return; 
    }
    if(param =="WiFi Reset") {    
        gSte->features.wifiReset = (uint8_t)(value);
        return; 
    }    
    if(param =="WiFi Enable") {    
        gCon->features.wifi_enabled = (int)(value);
        return;
    }
    if(param =="Startup Mode") {    
        gCon->features.startUpmode = (uint8_t)(value);
        return;
    }
    if(param =="Refresh Rate") {    
        gCon->features.refreshRate = (uint8_t)(value);
        return;
    }
    if(param =="Filter Type") {    
        gCon->features.filterType = (uint8_t)(value);
        return;
    }
    if(param =="Rotation") {    
        gCon->screen[0].rotation = (uint8_t)(value);
        gCon->screen[1].rotation = (uint8_t)(value);
        gCon->screen[2].rotation = (uint8_t)(value);
        return;
    }
    if(param =="Brightness") {    
        gCon->screen[0].brightness = value;
        gCon->screen[1].brightness = value;
        gCon->screen[2].brightness = value;
        return;
    }
    if(param =="HUB Mode") {    
        gCon->features.hubMode = (uint8_t)(value);
        return;
    }
    if(param =="Restore Default") {    
        gSte->system.resetToDefault = (uint8_t)(value);        
        return;
    }
    return;
}

void resetAutoTimer(){
    lastButtonActivity = millis();
}