# GNSS LoRaWAN tracker

This repository contains a project of a tracker device, which uses GNSS to obtain location and LoRaWAN as connectivity to upload tracker messages to the cloud.
This project uses Zephyr as RTOS.

![Project board](/Photos/photo_1.jpg)


## Main features

The main features of this project are:

* Obtain latitude and longitude coordinates generated by a GPS/GNSS module over UART (9600/8/N/1). All GPS/GNSS modules that use NMEA messages will do.
* Send LoRaWAN messages (ABP mode) using Smart Modular LoRaWAN module (https://ziliatech.com/iot/), which bahaves like a modem, controlled via AT commands (9600/8/N/1).
* Reads ignition status (on/off) and time spent with ignition on (in milisseconds)
* Reads GNSS fix status
* All logs messages are handled by Zephyr RTOS Logging subsystem
* Periodically send a LoRaWAN message containing latitude (4 bytes, float format), longitude (4 bytes, float format) and a status byte. 
This status byte contains ignition status (on/off) at bit 0 and GNSS fix status at bit 1.
The time interval between sending two consecutive LoRaWAN messages can be adjusted by CONFIG_GNSS_MANAGER_TIME_TO_SEND_LORAWAN_MESSAGE (see Kconfig for more details)

## LoRaWAN credentials

Before successfully send any LoRaWAN message, you need to set LoRaWAN credentials. As this projects uses LoRaWAN in ABP mode, the following credentials must be set:

* Device address: set this in CONFIG_LORAWAN_UART_SMARTMODULAR_DEVADDR (set Kconfig for more details)
* Application Session Key: set this in CONFIG_LORAWAN_UART_SMARTMODULAR_APPSKEY (set Kconfig for more details)
* Network Session Key: set this in CONFIG_LORAWAN_UART_SMARTMODULAR_NWKSKEY (set Kconfig for more details)
* Application EUI:  set this in CONFIG_LORAWAN_UART_SMARTMODULAR_APPEUI (set Kconfig for more details)

## Compatible hardwares

At this moment, there are oveelays that makes it available to compile this project for the following microcontrollers:

* ESP32 WROOM 32 (Xtensa architecture)

For ESP32 WROOM 32, wire the peripherals as described below:

- Breathing Light (led): GPIO 18
- Ignition switch: GPIO 4
- GNSS module (UART 1): GPIO 25 (TX) and GPIO 26 (RX)
- LoRaWAN module (UART 2): GPIO 23 (TX) and GPIO 5 (RX)

## How to compile this project

Assuming this code is placed into zephyrproject/zephyr folder, you can compile it by doing the following:

* For ESP32 WROOM, use the following command for compiling this project:

``
west build -p always -b esp32_devkitc_wroom gnss_lorawan_tracker/
``

And for flashing, simply use:

``
west flash
``