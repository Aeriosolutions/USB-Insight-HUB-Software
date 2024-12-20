#ifndef MasterStateService_h
#define MasterStateService_h

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


#include <EventSocket.h>
#include <HttpEndpoint.h>
#include <EventEndpoint.h>
#include <WebSocketServer.h>

#define DEFAULT_POWER_STATE false
#define OFF_STATE "OFF"
#define ON_STATE "ON"

#define LIGHT_SETTINGS_ENDPOINT_PATH "/rest/masterState"
#define LIGHT_SETTINGS_SOCKET_PATH "/ws/masterState"
#define LIGHT_SETTINGS_EVENT "master"

class MasterState
{
public:
    bool powerOn;
    bool switchOn;

    static void read(MasterState &settings, JsonObject &root)
    {
        root["power_on"] = settings.powerOn;
        root["switch_on"] = settings.switchOn;
    }

    static StateUpdateResult update(JsonObject &root, MasterState &masterState)
    {
        masterState.powerOn = root["power_on"] | false;
        masterState.switchOn = root["switch_on"] | false;
        return StateUpdateResult::CHANGED;
    }

};

class MasterStateService : public StatefulService<MasterState>
{
public:
    MasterStateService(PsychicHttpServer *server,
                      EventSocket *socket,
                      SecurityManager *securityManager);

    void begin();

private:
    HttpEndpoint<MasterState> _httpEndpoint;
    EventEndpoint<MasterState> _eventEndpoint;
    WebSocketServer<MasterState> _webSocketServer;

    void onConfigUpdated();
};

#endif