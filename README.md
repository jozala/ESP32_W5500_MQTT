ESP32_W5500_MQTT
================

Simple example of using Arduino Ethernet library with ESP32 and W5500 to subscribe and publish to MQTT broker.

IMPORTANT: 
**It has been created to present intermittent connection problems which seems to occur on W5500, but not on WiFi connection.**

## Physical connection between ESP32 and W5500

* GPIO23 <--> MOSI
* GPIO19 <--> MISO
* GPIO18 <--> SCLK
* GPIO5  <--> SCS
* GPIO11 <--> RESET

It is possible to change pins used for chip-select and reset in 
`include/MasterConfig.h` file.


## Configuration

Fixed IP address of the device (`ETHERNET_IP`) and MQTT hostname (`MQTT_HOSTNAME`) should most probably be changed in `include/MasterConfig.h` to fit your setup.
Changing other configurations in `MasterConfig.h` is optional.


## Running the test

It has been prepared as PlatformIO project. All dependencies will be automatically downloaded if run with PlatformIO.

Script for publishing messages (so ESP32 has something to read) has been prepared with Bash and Docker. That means it Docker is required for it and it is simple to run on Linux. It may also work with Windows Subsystem for Linux but it has not been tested.

1. Load and run project on ESP32
2. Start script `scripts/mqtt_pub_test.sh [MQTT_HOST]`
3. Observe serial output from ESP32

### Expected results:
- `Sending uptime message to MQTT: mydevice/uptime <- 15` is visible on serial output periodically,
- `Message arrived [mydevice/listen] 2019-06-20T14:25:11` is visible on serial output periodically
- above should happen indefinitely (until network is available).


### Actual results:

- ESP32 gets disconnected from network after some time (time periods differs significantly - from few minutes to couple of hours).

- Log `ERROR: mqttClient has been disconnected. Reconnecting...` is visible on serial output.

Log `Sending uptime message to MQTT` might still be visible on ESP32 serial output, but message with uptime is not send to MQTT broker at that time. This can be observed by subscribing to MQTT broker on `mydevice/uptime` topic. Subscribing to the topic can be easily achieved with `scripts/mosquitto_sub.sh -h [MQTT_HOST] -t "mydevice/uptime"` (requires Bash and Docker).