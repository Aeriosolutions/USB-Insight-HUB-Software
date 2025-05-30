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

#ifndef MENUVIEW_H
#define MENUVIEW_H

#include <Arduino.h>
#include "Buttons.h"
#include "datatypes.h"
#include "Screen.h"
#include "DefaultView.h"
#include "Extercomms.h"

#include <vector>
#include <string>
#include <functional>
#include <stack>
#include <memory>

#define MENU_VIEW_PERIOD      40 //in ms
#define AUTO_EXIT_TIMEOUT     10000 //in ms. 0 dispables the auto timer 


#define TYPE_ROOT    0 //Is menu item that has submenus
#define TYPE_SELECT  1 //Menu item that has a list of otions, stated in parameters
#define TYPE_RANGE   2 //Menu item that has range selection
#define TYPE_INFO    3 //Information box

//Help Reference index definiion

#define H_NONE       0 //No help text
#define H_CHxCONF    1 //Channel conf help text
#define H_GLOCONF    2 //Global conf help text
#define H_OC         3 //Over current Help
#define H_RC         4 //Reverse current help
#define H_CHSTUP     5 //Channel starup help

#define H_WIGEN      10 //wifi general help
#define H_WIREC      11 //wifi recovery info
#define H_WIRExNA    12 //wifi no action
#define H_WIRECREC   13 //wifi recovery
#define H_WIEN       14 //wifi eneable help
#define H_WIENNO     15 //wifi eneable help
#define H_WIENYES    16 //wifi eneable help
#define H_WIRES      17 //wifi reset help
#define H_WIRESRES   18 //wifi reset help

#define H_METER      20 //meter info
#define H_METREF     21 //meter refresh rate
#define H_METFILT    22 //meter filter type
#define H_METFILTMA  23 //moving average
#define H_METFILTMED 24 //median average

#define H_GLSTUP     28 //global config startup mode help
#define H_GLSTUPPER  29 //persistance mode
#define H_GLSTUPON   30 //all on mode
#define H_GLSTUPOFF  31 //all off mode
#define H_GLSTUPTMR  32 //use startup timer

#define H_SCREEN     35 //screen help
#define H_SCRROT     36 //screen rotation
#define H_SCRBRI     37 //screen brightness

#define H_USBTYP     40 //USB hub mode help
#define H_USBTYP23   41 
#define H_USBTYP2    42
#define H_USBTYP3    43 

#define H_DEF        45 //restore default help
#define H_DEFxNA     46 //restore default no action
#define H_DEFRES     47 //restore default action

struct Menu {
    int menuType;
    String name;               //Menu name
    std::vector<Menu> submenus;     //Submenus
    std::vector<String> params;     //parameters
    String paramUnits;    
    std::vector<int> helpReference;              //index to a specific help text  
    //std::function<void(String param)> setParam;
    //std::function<String()> getparam;   
};


void menuViewStart(GlobalState* globalState, GlobalConfig* globalConfig, Screen *screen);

void screenMenuIntroRender(Menu* m, Screen* s, String channel ="0");
void screenMenuListRender(Menu* m, Screen* s, int index,int type);
void screenMenuRangeRender(uint16_t value, String units, Screen* s);
void screenMenuInfoRender(Menu* m, Screen* s, uint16_t sel, int index=0);
void menuTextItemPlacer(String text, Screen* s, int pos, int selType, int tick);
void menuButtonTextPlacer(Screen* s, String barText);
void drawTextWithNewlines(Screen* s, const char* text, int startX, int startY, int textHeight);
void renderDemoScreen(Screen* s, int ch, int index=0);


#endif //MENUVIEW_H