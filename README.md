# USB Insight Hub - Software Components
**A USB interfacing tool for developers & tech enthusiasts**

## Overview 
USB Insight Hub supercharges the capabilities of a regular USB 3.0 Hub to provide special features designed for developers and tech enthusiasts: it plugs into your computer through a USB Type-C connector and exposes three USB 3.0 downstream ports, each with a 1.3-inch screen that displays relevant information about the attached device. That information includes operating system enumeration name (COMx or drive letter), voltage, and current. 

USB Insight Hub also allows you to control the individual activation and deactivation of power and the D+/D- USB 2 data lines to force enumeration on each downstream device. A small application (agent) running on the host computer extracts USB information from the operating system and sends it to the Hub over USB.
A full list of characteristics you can find in the [Hardware repository]( https://github.com/Aeriosolutions/USB-Insight-HUB-Hardware).

## Software components

The structure of the software that enables the operation of USB Insight Hub (UIH) is divided into three main parts, each one with a different development environment:

### UIH Enumeration Extraction Agent - Software

This program runs in the host computer where the USB Insight Hub is connected. This agent runs in the background of the operating system and every time the device tree is updated due to a USB change, it checks if the change happens in one of the three ports of the hub and, if is the case, extract the enumeration information of the new device and sends it to the ESP32 in the hub via a virtual USB-CDC link.
This program does not monitor or sniff the USB communication with any USB devices apart with the ESP32

### UIH-ESP32S3 - Firmware
This firmware handles the UIH main logic in the UIH hardware. The main tasks are:
- Stablishes and handles communication with the UIH Enumeration Extraction Agent through the USB-CDC link.
- Handles the on-board displays in the hardware.
- Provides a wireless interface (WiFi) to control and configure UIH and firmware updates.
- Retrieves and processes the information of peripherals: Power monitor (PAC1943), IO Extender (STM8), buttons and ADC.
- Direct connection to the STEMMA QT connector and spare Interface header pins for future expansion.


### UIH-STM8S003K3 - Firmware
This is a small microcontroller used mainly as an IO extender to the ESP32 and was a convenient solution to decrease the necessary connections between the BASER board and the Interface board. The main functions are:
- Handles a custom protocol, bidirectional I2C communication with the ESP32. 
- Controls the pins to set the short circuit current each USB power switch.
- Reads the fault status of the USB power switches.
- Controls the Data multiplexers for USB 2 signals in each channel.
- Automatically detects the USB type C direction to set the correct positions of the USB 3 upstream multiplexer. 
- Controls the enable signal of the USB 3 upstream multiplexer.
- Reads the analog signals of the voltage dividers of the USB PD (Power delivery) to know the power capabilities.
- Reads the status of the internal power commuter.
