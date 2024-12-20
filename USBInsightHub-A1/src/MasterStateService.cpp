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

#include <MasterStateService.h>

MasterStateService::MasterStateService(PsychicHttpServer *server,
                                     EventSocket *socket,
                                     SecurityManager *securityManager) :    _httpEndpoint(MasterState::read,
                                                                                            MasterState::update,
                                                                                            this,
                                                                                            server,
                                                                                            LIGHT_SETTINGS_ENDPOINT_PATH,
                                                                                            securityManager,
                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                            _eventEndpoint(MasterState::read,
                                                                                            MasterState::update,
                                                                                            this,
                                                                                            socket,
                                                                                            LIGHT_SETTINGS_EVENT),
                                                                            _webSocketServer(MasterState::read,
                                                                                            MasterState::update,
                                                                                            this,
                                                                                            server,
                                                                                            LIGHT_SETTINGS_SOCKET_PATH,
                                                                                            securityManager,
                                                                                            AuthenticationPredicates::IS_AUTHENTICATED)
{
    // configure led to be output
    ESP_LOGI("MasterState","Setup power_on");

    // configure settings service update handler to update LED state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);
}

void MasterStateService::begin()
{
    pinMode(LED_BUILTIN, OUTPUT);

    _httpEndpoint.begin();
    _eventEndpoint.begin();
    _state.powerOn = DEFAULT_POWER_STATE;
    _state.switchOn = false;
    onConfigUpdated();
}

void MasterStateService::onConfigUpdated()
{
    digitalWrite(LED_BUILTIN, _state.powerOn ? 1 : 0);
    //ESP_LOGI("MasterState","Power Button changed to %s",_state.powerOn ? "on" : "off");
    //ESP_LOGI("MasterState","Switch Input changed to %s",_state.switchOn ? "on" : "off");
}

