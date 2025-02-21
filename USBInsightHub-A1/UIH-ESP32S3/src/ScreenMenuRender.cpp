#include "Screen.h"
#include "MenuView.h"
#include <WiFi.h>

const char* helpArr[] = {
/*   "Individual Channel"*/    
/*0*/"",
/*1*/"Individual CH:\n -Over Current\n -Back Current\n -Starup Timer",
/*2*/"Global settings:\n -Wi-Fi\n -Startup Mode\n -Meter\n -Screen\n -Hub Mode",
/*3*/"Forward current\nlimit",
/*4*/"Reverse current\nlimit",
/*5*/"Delay time\nafter power up\nto turn on this\nchannel",
/*6*/"",
/*7*/"",
/*8*/"",
/*9*/"",
/*10*/" -Information\n -Recovery \n -Reset\n -Enable",
/*11*/"Reset the current\nconnection,\nand force\nAccess Point\nmode",
/*12*/"No action",
/*13*/"Reset the current\nconnection,\nand force\nAccess Point\nmode",
/*14*/"Enable or disable\nWi-Fi.\n\nUNIT RESETS\nAFTER CHANGE!",
/*15*/"Disable Wi-Fi.\nConnection\nconfigurations\nare preserved",
/*16*/"Enable Wi-Fi.\nFor first time\njoin network\nUSB-Insight-Hub\nPass: usb-insight",
/*17*/"Factory reset.\nDeletes all\nnetwork\nconfigurations",
/*18*/"Factory reset.\nDeletes all\nnetwork\nconfigurations!\nUNIT RESETS!",
/*19*/"",
/*20*/"Current and\nVoltage meter\nconfigs",
/*21*/"Meter refresh\nrate.",
/*22*/"Type of digital\nfilter applied to\nthe voltage and\ncurrent samples",
/*23*/"The average of\nthe last 10 or 20\nsamples",
/*24*/"Sort the last\n10 or 20 samples\nand get the\nmedian value",
/*25*/"",
/*26*/"",
/*27*/"",
/*28*/"Select the logic\nto control\noutputs at\npower up",
/*29*/"Each output\nremembers the\nstate before\npower turns off ",
/*30*/"All outputs\nturn on after\npower up",
/*31*/"All outputs\nremain off after\npower up",
/*32*/"Each channel\nturns on based\non its startup\ntimer",
/*33*/"",
/*34*/"",
/*35*/" -Rotation\n -Brightness",
/*36*/"Screen rotation",
/*37*/"Screen\nbrightness",
/*38*/"",
/*39*/"",
/*40*/"Select the USB\noperation mode \nof the USB HUB",
/*41*/"USB 2\nLow, Full, High\nand USB 3\nSuper-Speed\nenabled",
/*42*/"USB 2\nLow, Full, High\nSpeeds only",
/*43*/"USB 3\nSuper-Speed only",
/*44*/"",
/*45*/""
};


void screenMenuIntroRender(Menu* m, Screen* s, String channel){

    uint8_t ch=0;

    digitalWrite(s->dProp[ch].cs_pin, LOW);     
    //s->tft.setRotation(s->dProp[ch].rotation);
    s->tft.setRotation(ROT_180_DEG);
    s->img.fillScreen(TFT_BLACK);          

    //header
    s->img.fillRoundRect(5,17,230,6,1,TFT_LIGHTGREY);
    if(m->name =="Over Current" || m->name =="Back Current" ||m->name =="Startup Timer"){
        menuTextItemPlacer("CH"+String(channel.toInt()+1),s,1,0,0);
        menuTextItemPlacer(m->name,s,2,0,0);
        menuButtonTextPlacer(s,"Return");
    }
    else if(m->name == "Configurations"){
        menuTextItemPlacer(m->name,s,1,0,0);
        menuTextItemPlacer("Ver: " + String(APP_VERSION) + "_" + channel,s,3,0,0);
        String mac = WiFi.macAddress();
        mac.replace(":", "");  // Remove colons
        menuTextItemPlacer(mac,s,4,0,0);
    }
    else{
        menuTextItemPlacer(m->name,s,1,0,0);
        menuButtonTextPlacer(s,"Return");
    }

    s->img.pushSprite(0,0);
    digitalWrite(s->dProp[ch].cs_pin, HIGH);
}

void screenMenuListRender(Menu* m, Screen* s, int index, int type){
    uint8_t ch=1;
    String txt="";
    bool downArrow = false;

    digitalWrite(s->dProp[ch].cs_pin, LOW);     
    //s->tft.setRotation(s->dProp[ch].rotation);
    s->tft.setRotation(ROT_180_DEG);
    s->img.fillScreen(TFT_BLACK); 

    //header
    s->img.fillRoundRect(5,17,230,6,1,TFT_LIGHTGREY);

    int start = 0;
    int size = 0;
    type < 0 ? size = m->submenus.size(): size = m->params.size();

    //down arrow if more items exist
    if(size > 4 && (index+1) != size) downArrow = true;   

    if(size > 4) size = 4;
    index < 4 ? start = 0 : start = index -3;
    //displace the list
    for(int i=start; i < (start+size); i++){
        if(type < 0){
            txt = String(i+1) + "." + m->submenus[i].name;
            menuTextItemPlacer(txt,s,(i-start+1),index==i ? 1:0,0);
        } 
        else {
            txt = String(i+1) + ". " + m->params[i];
            menuTextItemPlacer(txt,s,(i-start+1),index==i ? 1:0, type==i ? 1:0);                
        }
    }   

    if(downArrow) s->img.fillTriangle(210,170,230,170,220,190,TFT_LIGHTGREY);

    menuButtonTextPlacer(s,"Move");

    s->img.pushSprite(0,0);
    digitalWrite(s->dProp[ch].cs_pin, HIGH);
}


void screenMenuRangeRender(uint16_t value, String units, Screen* s){
    uint8_t ch=1;

    digitalWrite(s->dProp[ch].cs_pin, LOW);     
    //s->tft.setRotation(s->dProp[ch].rotation);
    s->tft.setRotation(ROT_180_DEG);
    s->img.fillScreen(TFT_BLACK); 

    //header
    s->img.fillRoundRect(5,17,230,6,1,TFT_LIGHTGREY);

    s->img.loadFont(aptossb52l);
    s->img.setTextSize(2);
    s->img.setTextColor(TFT_WHITE);
    if(units =="s"){        
        s->img.drawCentreString(String((float)(value)/10,1), 120, 80, 4);    
    }
    else if(units =="%"){
        s->img.drawCentreString(String((float)(value)/10,0), 120, 80, 4); 
    }
    else {
        s->img.drawCentreString(String(value), 120, 80, 4);
    }
    //aux.concat(" V");    
    
    s->img.drawCentreString(String(units), 120, 120, 4);    
    s->img.unloadFont();
    
    menuButtonTextPlacer(s,"Down");

    s->img.pushSprite(0,0);
    digitalWrite(s->dProp[ch].cs_pin, HIGH);        
}

void screenMenuInfoRender(Menu* m, Screen* s, uint16_t sel ,int index){
    uint8_t ch=2;

    digitalWrite(s->dProp[ch].cs_pin, LOW);     
    //s->tft.setRotation(s->dProp[ch].rotation);
    s->tft.setRotation(ROT_180_DEG);
    s->img.fillScreen(TFT_BLACK); 

    //header
    s->img.fillRoundRect(5,17,230,6,1,TFT_LIGHTGREY);

    s->img.loadFont(SMALLFONT);
    s->img.setTextFont(2);
    s->img.setTextColor(TFT_WHITE);

    if(m->menuType == TYPE_ROOT){        
        drawTextWithNewlines(s,helpArr[m->submenus[index].helpReference[0]],5,30,3);
    }
    if(m->menuType == TYPE_RANGE){
        drawTextWithNewlines(s,helpArr[m->helpReference[0]],5,30,3);
        if(m->name == "Brightness"){
            s->screenSetBackLight(sel);
            renderDemoScreen(s, ch, ROT_180_DEG);
            digitalWrite(s->dProp[ch].cs_pin, LOW);
        }
    }
    if(m->menuType == TYPE_SELECT){        
        drawTextWithNewlines(s,helpArr[m->helpReference[index+1]],5,30,3);
        if(m->name == "Rotation"){
            renderDemoScreen(s, ch, index);
            digitalWrite(s->dProp[ch].cs_pin, LOW);
            //elements draw directly on screen to keep 
            s->tft.setRotation(ROT_180_DEG);
            s->tft.fillRect(0,195,240,45,TFT_BLACK);
            s->tft.fillRoundRect(0,200,240,40,5,TFT_LIGHTGREY);
            s->tft.fillRoundRect(2,202,236,36,5,DARKGREY);
            s->tft.loadFont(SMALLFONT);
            s->tft.setTextFont(2);
            s->tft.setTextColor(TFT_WHITE);        
            s->tft.drawCentreString("Select",120,210,4);
            digitalWrite(s->dProp[ch].cs_pin, HIGH);
            return;       
            //s->tft.setRotation                        
        } 
    }    

    if(m->menuType == TYPE_SELECT || m->menuType == TYPE_ROOT)
        menuButtonTextPlacer(s,"Select");
    if(m->menuType == TYPE_RANGE)
        menuButtonTextPlacer(s,"Up");

    s->img.unloadFont();
    s->img.pushSprite(0,0);
    digitalWrite(s->dProp[ch].cs_pin, HIGH);
}

//****************************** HELPERS ********************************* */

void menuTextItemPlacer(String text, Screen* s, int pos, int selType, int tick){

    int32_t x = 6;
    int32_t y = 40*pos+5;

    if(selType == 1){
        s->img.fillRoundRect(2,40*pos-3,236,36,5,DARKGREY);
    }

    if(tick == 1){
        s->img.fillSmoothCircle(227,40*pos+15,9,TFT_LIGHTGREY,TFT_LIGHTGREY);
        s->img.fillSmoothCircle(227,40*pos+15,5,TFT_WHITE,TFT_WHITE);
    }   

    s->img.loadFont(SMALLFONT);
    s->img.setTextFont(2);
    s->img.setTextColor(TFT_WHITE);

    s->img.drawString(text,x,y,4);
    s->img.unloadFont();

}

void menuButtonTextPlacer(Screen* s, String barText){

    //rectangle color    
    s->img.fillRoundRect(0,200,240,40,5,TFT_LIGHTGREY);
    s->img.fillRoundRect(2,202,236,36,5,DARKGREY);
    s->img.loadFont(SMALLFONT);
    s->img.setTextFont(2);
    s->img.setTextColor(TFT_WHITE);        
    s->img.drawCentreString(barText,120,210,4);
    s->img.unloadFont();
}

void drawTextWithNewlines(Screen* s, const char* text, int startX, int startY, int textHeight) {
  s->img.setCursor(startX, startY); // Set initial position
  const char* ptr = text;

  while (*ptr) {
    if (*ptr == '\n') {
      startY += textHeight * 9; // Adjust for line spacing (8 pixels per text size unit)
      s->img.setCursor(startX, startY); // Move to the next line
    } else {
      s->img.print(*ptr); // Print the character
    }
    ptr++;
  }

}

void renderDemoScreen(Screen* s, int ch, int index){
    chScreenData demosScr = {
        {},
        {5000,725,1000,20,false,false},
        {1,"COMx","",3},
        false, true, true, ILIM_1_0, true, 0,0
    };

    demosScr.dProp.cs_pin = s->dProp[ch].cs_pin;
    demosScr.dProp.rotation = index;

    s->screenDefaultRender(demosScr);

}