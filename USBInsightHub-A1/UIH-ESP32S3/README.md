# USB Insight Hub - ESP32-S3 Firmware


## 1. Framework
The project is developed using VSC, with Platformio IDE and Arduino. The Espresif32 platform compatibility is 6.6.0 (Arduino v2.0.14 and ESP-IDF v5.2.1). It is recommended to keep these versions as updating them breaks compatibility with some libraries (for example display library TFT_eSPI).

USB Insight Hub project is based on [ESP32 Sveltekit project](https://theelims.github.io/ESP32-sveltekit/) and the instructions on how to set up the project are well documented [here](https://theelims.github.io/ESP32-sveltekit/gettingstarted/).  It is highly advisable to first try to compile the [example]( https://github.com/theelims/ESP32-sveltekit) provided by the ESP32 Sveltekit before compiling UIH-ESPS3 project.

The UIH logic runs on core 1 and the Web server / WiFi functions run on core 0.
The specific configurations for the project are defined in the files platformio.ini, features.ini and factory_settings.ini, using also a custom partition table partition_uih_8MB which.csv. which is tailored to have a relatively small spiffs data partition (524k) and two app partitions of 3.8MB. 

Although the intention was not to modify the ESP32 Sveltekit backend library, it was necessary to make some modification for UIH work properly: 
- Increase the number of sockets that can be handled at the same time from 6 to the maximum 13 (that Arduino allows) in framework\ESP32SvelteKit.cpp. 
- Increase the maximum upload size of files from OTA from 2.3MB  to 3.8MB in framework \UploadFirmwareService.cpp.
- Enable hashing of frontend files to force browser cache refresh (if this in not done, every time a change is made in the frontend files a manual browser cache cleaning is necessary).

**Libraries used:**
- [ESP32 SvelteKit](https://github.com/theelims/ESP32-sveltekit)
- [TFT-eSPI](https://github.com/Bodmer/TFT_eSPI)
- [ArduinoJSON](https://arduinojson.org/)

## 2. Programming
### Direct flashing by serial
To update the firmware and have access to the debug serial prints, use header J4 in the Interface board. A small adapter is necessary to interface with an ESP32 compatible programmer, like the ESP-Prog (used in this project). It is recommended to use [ESP Flash tool](https://dl.espressif.com/public/flash_download_tool.zip) if the full .bin file is uploaded, or in development environment use the included Upload tool in Platformio.

Check the [UIH Programing Guide](https://github.com/Aeriosolutions/USB-Insight-HUB-Software/tree/main/USBInsightHub-A1/)

USB_CDC_ON_BOOT is disabled by default in order not to interfere with the USB communication with the Enumeration Extraction Agent.

### Firmware updating
A simpler option, that does not require additional hardware, is to use the OTA facility provided in the web interface. Every time the project is compiled successfully, the resulting binary in the build/firmware directory can be uploaded to the unit. After uploading the file, the unit will boot and the new version will become active.

## 3. Hardware Drivers
### Display
The three displays are controlled by TFT-eSPI driver, which is part of the project as the library has some necessary modification:
- TFT_Drivers/ST7789_init.h – It has a personalized configuration for the 1.3” 240x240 display initialization based on the manufacturer recommendation.
- User_Setup.h. – Is the provided file by the library to select the controller, driver, resolution and pin definition for SPI communication. 

Special considerations:

Each display is refreshed every 190ms in a round robin fashion and this limits the overall refresh rate to 5.2 fps. Though this is a low value, it is responsive enough for the application. Increasing the refresh rate starves core 1 (where UIH logic resides) and doing so does not bring evident benefits. 8 bit color scheme is used to save SRAM space, which is not supported to be used with DMA.  

### PAC1943
Microchip does not provide an Arduino library for this chip (at March 2024) but a comprehensive and extensive generic cpp [library]( https://ww1.microchip.com/downloads/aemDocuments/documents/MSLD/ProductDocuments/SoftwareLibraries/Firmware/pac194x_5x_generic_lib_v1_0.zip) is available for reference. An in-house implementation is used (with the functions necessary) to communicate with this chip via I2C using the Wire library from Arduino and the interrupt signal (used to detect overcurrent and reverse current alerts) is handled in the main I2C handling loop.

### STM8 (as IO extender)
In the Base Board resides an 8 bit STM8 used as IOExtender and USB control signal monitor. The communication is by I2C in a shared channel with PAC1943 and a driver is provided for this communication. Please check the README file in UIH-STM8S003K3 folder for details on the protocol, implementation and programming.



