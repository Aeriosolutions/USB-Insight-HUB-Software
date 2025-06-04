export const Help = {
    CONTROL: {
        HOSTLINK: "USB 2 connection status between ESP32 and the host computer",
        HOSTCOM: "Communication status between the ESP32 and the agent program in the host computer",
        POWER: "Hub is powered by the HOST or the AUX power supply",
        OVER_CURRENT: "Forward current limit. From the Hub to the load",
        BACK_CURRENT: "Reverse current limit. From the load to the Hub",
        STARTUP_DELAY: "Delay time after power-on to turn on the channel power. Settings->Startup Mode->Timed must be selected",
    },
    SETTINGS: {
        STARTUP_MODE: "Select the logic to control the outputs at power up:\n"+
            "1- PERSISTANCE:      Each output remembers the state before power turns off\n" +
            "2- ON AT STARTUP:  All outputs turn on after power up\n" +
            "3- OFF AT STARTUP: All outputs remain off after power up\n" +
            "4- TIMED:                 Each channel turns on based on its startup timer",        
        METER: {             
            REFRESH_RATE:"Lower values provides faster refresh rate but a shorter filter",
            FILTER_TYPE: "Type of digital filter applied to the current and voltage samples:\n" +
            "1- AVERAGE: Average of the last samples in the buffer (depends on refresh rate)\n"+
            "2- MEDIAN:  Sort the samples in the buffer and get the median value",
        },
        SCREEN: {
            ROTATION:"Rotate the screen based on Hub aligement",
            BRIGHTNESS: "Change brightness of all screens",
        },
        HUB_MODE: "Enable/Disable different USB communication lines:\n"+
            "1- USB2_3:   USB 2 Low, Full, High and USB 3 Super-Speed enabled\n"+
            "2- USB2:       USB 2 Low, Full, High Speeds only\n"+
            "3- USB3:       USB 3 Super-Speed only"
        ,
    },
};