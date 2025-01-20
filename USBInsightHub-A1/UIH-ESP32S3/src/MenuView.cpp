#include "MenuView.h"

GlobalState *gSte;
GlobalConfig *gCon;
Screen *iScr;

static const char* TAG = "MenuView";

uint8_t mLevel = 0; //menu level
uint8_t mIndex = 0; //menu Index (current pointed element)
bool treeEnd = false;

Menu* currentMenu;
std::stack<Menu*> menuStack;

Menu CH1Config = {
    TYPE_ROOT,
    "CH1 Config.",
    {
        {TYPE_RANGE,"Over Current",   {},{"100","2000","100"},"",H_OC},
        {TYPE_RANGE,"Reverse Current",{},{"1","200","10"},"",H_RC},
        {TYPE_RANGE,"Startup Timer",  {},{"1","100","5"},"",H_CHSTUP}
    },{"0"},"",H_CHxCONF
};

Menu CH2Config = {
    TYPE_ROOT,
    "CH2 Config.",
    {
        {TYPE_RANGE,"Over Current",   {},{"100","2000","100"},"",H_OC},
        {TYPE_RANGE,"Reverse Current",{},{"1","200","10"},"",H_RC},
        {TYPE_RANGE,"Startup Timer",  {},{"1","100","5"},"",H_CHSTUP}
    },{"1"},"",H_CHxCONF
};

Menu CH3Config = {
    TYPE_ROOT,
    "CH3 Config.",
    {
        {TYPE_RANGE,"Over Current",   {},{"100","2000","100"},"",H_OC},
        {TYPE_RANGE,"Reverse Current",{},{"1","200","10"},"",H_RC},
        {TYPE_RANGE,"Startup Timer",  {},{"1","100","5"},"",H_CHSTUP}
    },{"2"},"",H_CHxCONF
};

Menu generalConfig = {
    TYPE_ROOT,
    "Global Config.",
    {
        {TYPE_ROOT, "Wi-Fi",{
            {TYPE_INFO,"WiFI Information",{},{},"",H_NONE},
            {TYPE_SELECT,"WiFi Reset",{},{"No Action", "Reset"},"",H_WIRES},
            {TYPE_SELECT,"WiFi Enable",{},{"Disable", "Enable"},"",H_WIEN}
        },{},"",H_WIGEN},
        {TYPE_SELECT, "Startup Mode",{},{"Persistance", "On at startup", "Off at startup", "Timed sequence"},"",H_GLSTUP},
        {TYPE_ROOT, "Meter",{
            {TYPE_SELECT,"Refresh Rate",{},{"0.5s","1.0s"},"",H_METREF},
            {TYPE_SELECT,"Filter Type",{},{"Moving Average","Median"},"",H_METFILT}
        },{},"",H_SCREEN},        
        {TYPE_ROOT, "Screen",{
            {TYPE_SELECT,"Rotation",{},{"0","90","180","270"},"",0},
            {TYPE_RANGE,"Brightness",{},{"100","1000","50"},"",0}
        },{},"",H_SCREEN},
        {TYPE_SELECT, "HUB Mode",{},{"USB2 and USB3", "USB2 Only", "USB3 Only"},"",H_USBTYP}
    },
    {"0"},"", H_GLOCONF
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
    {"0"},"", 0
};

uint16_t  getParamValue(String parameter,String channel="0");
void setParamValue(String param, uint16_t value, String channel="0");

void rootLayout(Menu* root, int index);
void selectLayout(Menu* root, int index);
void rangeLayout(Menu* root, String channel="0");

void taskMenuViewLoop(void *pvParameters);


void menuViewStart(GlobalState* globalState, GlobalConfig* globalConfig, Screen *screen){

    gSte = globalState;
    gCon = globalConfig;
    iScr = screen;

    xTaskCreatePinnedToCore(taskMenuViewLoop, "MenuScreen", 4096, NULL, 4, NULL,APP_CORE);

}

void taskMenuViewLoop(void *pvParameters){
    ESP_LOGI(TAG,"Loop on Core %u",xPortGetCoreID());
    
    if(xSemaphoreTake(screen_Semaphore,pdMS_TO_TICKS(50) ) == pdTRUE)
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

        rootLayout(currentMenu,mIndex);

        for(;;){

            if(btnShortCheck(0)){                
                //Back button
                if(!menuStack.empty()){
                    currentMenu = menuStack.top();
                    menuStack.pop();
                    mIndex = 0;
                    treeEnd = false;
                    if(!currentMenu->params.empty()) ch = currentMenu->params[0];
                    rootLayout(currentMenu,mIndex);
                }
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
                        if(sel - step >= rmin){
                            sel = sel - step;
                            setParamValue(currentMenu->name,sel,ch);
                        }                         
                    }                     

                    rangeLayout(currentMenu,ch);                     
                }                                
            }
            if(btnShortCheck(2)){
                //Select
                
                //if(!currentMenu->submenus.empty()){
                if(!treeEnd){
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
                            sel = sel + step;
                            setParamValue(currentMenu->name,sel,ch);    
                        }                        
                    }                   

                    rangeLayout(currentMenu,ch);
                    treeEnd = true;                      
                }

            }
            if(btnShortCheck(3)){
                ESP_LOGI(TAG,"Setup key");
            }

            if (btnLongCheck(3)){
            
                ESP_LOGI(TAG,"Setup button long press");
                xSemaphoreGive(screen_Semaphore);
                defaultViewStart();
                ESP_LOGI(TAG,"Delete Task Loop");
                vTaskDelete(NULL);
            }

            vTaskDelay(pdMS_TO_TICKS(MENU_VIEW_PERIOD));
        }    
    } 
    else {
        ESP_LOGE(TAG, "Screen resource taken, could not initialize");
        defaultViewStart();
        ESP_LOGI(TAG,"Delete Task Loop");
        vTaskDelete(NULL);    
    }

}


void rootLayout(Menu* root, int index){
    
    ESP_LOGI("","-------------");
    ESP_LOGI("","%s",root->name.c_str());  
    //ESP_LOGI("","Type: %u",root->menuType);  
    ESP_LOGI("","-------------");
    for(int i=0; i< root->submenus.size(); i++){
        if(index == i)
            ESP_LOGI("","[%s]",root->submenus[i].name.c_str());
        else  
            ESP_LOGI(""," %s",root->submenus[i].name.c_str());
    }
    ESP_LOGI("","-------------");
    ESP_LOGI("","Help Index: %u", root->submenus[index].helpReference);
    
}

void selectLayout(Menu* root, int index){

    ESP_LOGI("","-------------");
    ESP_LOGI("","%s",root->name.c_str());    
    ESP_LOGI("","-------------");
    if(root->menuType == TYPE_SELECT){

        uint16_t sel = getParamValue(root->name);
        String strm ="";
        for(int i=0; i< root->params.size(); i++){            
            strm=root->params[i];
            if(sel == i) strm = strm + "*";
            if(index == i)                
                ESP_LOGI("","[%s]", strm.c_str());
            else  
                ESP_LOGI(""," %s" , strm.c_str());
        }
    }
    ESP_LOGI("","-------------");
    ESP_LOGI("","Help Index: %u", root->helpReference);
}

void rangeLayout(Menu* root, String channel){
    
    uint16_t sel = getParamValue(root->name, channel);
    ESP_LOGI("","-------------");
    ESP_LOGI("","%s",root->name.c_str());    
    ESP_LOGI("","-------------");
    ESP_LOGI("","<< %u >>",sel);
    ESP_LOGI("","-------------");
    ESP_LOGI("","Help Index: %u", root->helpReference);
}


uint16_t getParamValue(String param, String channel){

    if(param =="Over Current") {    
        return ((uint16_t)(gCon->meter[channel.toInt()].fwdCLim));
    }
    if(param =="Reverse Current") {    
        return ((uint16_t)(gCon->meter[channel.toInt()].backCLim));
    }
    if(param =="Startup Timer") {    
        return ((uint16_t)(gCon->startup[channel.toInt()].startup_timer));
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

    return (0);   
}

void setParamValue(String param, uint16_t value, String channel){
    if(param =="Over Current") {    
        gCon->meter[channel.toInt()].fwdCLim = value;
        return;
    }
    if(param =="Reverse Current") {    
        gCon->meter[channel.toInt()].backCLim = value;
        return;
    }
    if(param =="Startup Timer") {    
        gCon->startup[channel.toInt()].startup_timer = (int)(value);
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
        gCon->screen[channel.toInt()].rotation = (uint8_t)(value);
        return;
    }
    if(param =="Brightness") {    
        gCon->screen[channel.toInt()].brightness = value;
        return;
    }
    if(param =="HUB Mode") {    
        gCon->features.hubMode = (uint8_t)(value);
        return;
    }
    return;
}