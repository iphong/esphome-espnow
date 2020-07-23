# ESP-NOW Integration for ESPHome
This addon allow direct communication between your esphome devices.

## The Problem
* EspHome is no doubt the best platform for creating your own DIY smart devices, but it requires a WiFi router and HomeAssistant in order to communicate.

## Other solutions
* Using RF and IR module, but this requires additional hardwares

## Why ESP-NOW?
* Works even without WiFi connection
* Other WiFi functionality remains the same
* No addition hardware needed

## Usage

1. A main device contains simple gpio switch and a light

```yaml

esphome:
  ...
  arduino_version: LATEST
  includes:
  - esp_now_connect.h

output:
- platform: gpio
  pin: 13
  id: my_output

light:
- platform: binary
  output: my_output
  id: my_light
  name: My Light

switch:
- platform: gpio
  pin: 12
  id: my_switch
  on_turn_on:
  - lambda: ESPNow.send("my_switch on");
  on_turn_off:
  - lambda: ESPNow.send("my_switch off");

custom_component:
- lambda: |-
    ESPNow.begin(2);
    ESPNow.on("my_motion on", []() {
      id(my_light)->turn_on().perform();
    });
    ESPNow.on("my_motion off", []() {
      id(my_light)->turn_off().perform();
    });
    ESPNow.on("status my_switch", []() {
      if (id(my_switch).state) {
        ESPNow.send("my_switch on");
      } else {
        ESPNow.send("my_switch off");
      }
    });
    return {};

```

2. A second device contains a motion sensor

```yaml

esphome:
  ...
  arduino_version: LATEST
  includes:
  - esp_now_connect.h

binary_sensor:
- platform: gpio
  pin: 2
  id: my_motion
  on_press:
  - lambda: ESPNow.send("my_motion on");
  on_release:
  - lambda: ESPNow.send("my_motion off");

custom_component:
- lambda: |-
    ESPNow.begin(2);
    return {};

```

3. A third device contains a LED state indicator of the switch on device 1

```yaml

esphome:
  ...
  arduino_version: LATEST
  includes:
  - esp_now_connect.h

output:
- platform: gpio
  pin: 13
  id: my_output

light:
- platform: binary
  output: my_output
  id: my_light
  name: My Light

custom_component:
- lambda: |-
    ESPNow.begin(2);
    ESPNow.on("my_switch on", []() {
      id(my_light)->turn_on().perform();
    });
    ESPNow.on("my_switch off", []() {
      id(my_light)->turn_off().perform();
    });
    ESPNow.send("status my_switch");
    return {};

```
