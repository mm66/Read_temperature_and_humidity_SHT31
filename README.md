# Read temperature and humidity from SHT31 sensor

## Overview

This repository implements a RESTFUL API server and HTTP server on ESP32, it reads temperature and humidity values
from SHT31 sensor connected to the ESP32 board and displays them to the user in the browser (current reading and a chart).



## How to use project

### Hardware required

To run this project, you need and ESP32 dev board or ESP32 core board and a SHT31 temperature/humidity sensor.

#### PIN Assignment


| ESP32   | SHT31   |
| ------  | ------- |
| GPIO22  | SCL     |
| GPIO21  | SDA     |
| 3v3     | VIN     |
| GND     | GND     |

### Configure project

Open project configuration menu.

In `Example Connection Configuration` menu:

* Choose the network interface in 'Connect using' option. Select the Wi-Fi interface and your Wi-Fi SSID and Wi-Fi password of the network your ESP32 will connect to

### Build and Flash

Build the project and then flash it to the ESP32 board using a serial connection.

### Render webpage in browser

In your browser, enter the URL `http://esp-home.local`. You can also enter the IP address that the ESP32 obtained.
