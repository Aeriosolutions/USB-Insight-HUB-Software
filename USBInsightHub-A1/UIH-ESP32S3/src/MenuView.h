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

#define TYPE_ROOT    0 //Is menu item that has submenus
#define TYPE_SELECT  1 //Menu item that has a list of otions, stated in parameters
#define TYPE_RANGE   2 //Menu item that has range selection
#define TYPE_INFO    3 //Infomration box

#define H_NONE    0 //No help text
#define H_CHxCONF 1 //Channel conf help text
#define H_GLOCONF 2 //Global conf help text
#define H_OC      3 //Over current Help
#define H_RC      4 //Reverse current help
#define H_CHSTUP  5 //Channel starup help
#define H_METREF  6
#define H_METFILT 7
#define H_WIGEN   8 //wifi general help
#define H_WIRES   9 //wifi reset help
#define H_WIEN    10 //wifi eneable help
#define H_GLSTUP  11 //global config startup mode help
#define H_SCREEN  12 //screen help 
#define H_USBTYP  13 //USB hub mode help 


struct Menu {
    int menuType;
    String name;               //Menu name
    std::vector<Menu> submenus;     //Submenus
    std::vector<String> params;     //parameters
    String paramName;    
    int helpReference;              //index to a specific help text  
    //std::function<void(String param)> setParam;
    //std::function<String()> getparam;   
};


void menuViewStart(GlobalState* globalState, GlobalConfig* globalConfig, Screen *screen);

#endif