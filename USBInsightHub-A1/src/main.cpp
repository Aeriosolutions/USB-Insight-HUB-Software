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

#include <ESP32SvelteKit.h>
//#include <LightMqttSettingsService.h>
//#include <LightStateService.h>
#include <PsychicHttpServer.h>
#include <MasterStateService.h>

#include <ArduinoJson.h>

#define SERIAL_BAUD_RATE 115200

JsonDocument masterState;

PsychicHttpServer server;

ESP32SvelteKit esp32sveltekit(&server, 120);

/*
LightMqttSettingsService lightMqttSettingsService = LightMqttSettingsService(&server,
                                                                             esp32sveltekit.getFS(),
                                                                             esp32sveltekit.getSecurityManager());

LightStateService lightStateService = LightStateService(&server,
                                                        esp32sveltekit.getSocket(),
                                                        esp32sveltekit.getSecurityManager(),
                                                        esp32sveltekit.getMqttClient(),
                                                        &lightMqttSettingsService);
*/
MasterStateService masterStateService = MasterStateService(&server,
                                                        esp32sveltekit.getSocket(),
                                                        esp32sveltekit.getSecurityManager());                                                        

void setup()
{
    // start serial and filesystem
    Serial.begin(SERIAL_BAUD_RATE);

    // start ESP32-SvelteKit
    esp32sveltekit.begin();

    // load the initial light settings
    //lightStateService.begin();
    // start the light service
    //lightMqttSettingsService.begin();
    masterStateService.begin();
    deserializeJson(masterState, "{\"power_on\":\"false\",\"switch_on\":\"false\"}");   
}

void loop()
{
    JsonObject masterStateObj =masterState.as<JsonObject>();

    //read
    masterStateService.read(masterStateObj,MasterState::read);
    //change something
    bool power_on = (bool)(masterStateObj["power_on"]);
    bool switch_on = (bool)(masterStateObj["switch_on"]);
    //ESP_LOGI("Main","Power Button changed to %s",power_on ? "on" : "off");
    switch_on = !switch_on;
    //ESP_LOGI("Main","Switch changed to %s",switch_on ? "on" : "off");
    masterStateObj["switch_on"] = switch_on; 
    //update
    masterStateService.update(masterStateObj,MasterState::update,"main");
    delay(1000);

}
