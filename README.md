# ESP-NOW Integration for ESPHomE

ESP_RC IS NOW A STANDALONE LIBRARY AND HAS BEEN MOVED TO https://github.com/iphong/lib-esp-rc

THIS REPO WILL ONLY FOCUS ON ESPHOME & HOMEASSISTANT INTEGRATION

----------------

This addon allow direct communication between your esphome devices.

## The Problem
* EspHome is no doubt the best platform for creating your own DIY smart devices, but it requires a WiFi router and HomeAssistant in order to communicate.

## Other solutions
* Using BT, RF or IR module, but this requires additional hardwares

## Why esp-now?
* Works with or without WiFi connection
* Works with or without HomeAssistant
* Instant startup time if not using wifi
* No addition hardware needed
* Easy to use pub/sub event model
* Send and receive from multiple devices

## Usage examples

ATTENTION: Make sure to use the latest arduino version as this uses the new esp-now broadcast feature. And it's recommended to use the same channel as your wifi router.

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
  - lambda: EspRC.send("my_switch on");
  on_turn_off:
  - lambda: EspRC.send("my_switch off");

custom_component:
- lambda: |-
    EspRC.begin(2);
    EspRC.on("my_motion on", []() {
      id(my_light)->turn_on().perform();
    });
    EspRC.on("my_motion off", []() {
      id(my_light)->turn_off().perform();
    });
    EspRC.on("status my_switch", []() {
      if (id(my_switch).state) {
        EspRC.send("my_switch on");
      } else {
        EspRC.send("my_switch off");
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
  - lambda: EspRC.send("my_motion on");
  on_release:
  - lambda: EspRC.send("my_motion off");

custom_component:
- lambda: |-
    EspRC.begin(2);
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
    EspRC.begin(2);
    EspRC.on("my_switch on", []() {
      id(my_light)->turn_on().perform();
    });
    EspRC.on("my_switch off", []() {
      id(my_light)->turn_off().perform();
    });
    EspRC.send("status my_switch");
    return {};

```

### CHANGE LOGS:

Update 1:

1. To prevent conflict with EspRC namespace, i will use the namespace EspRC instead.
2. You can now receive and send message with arguments but this method is having performance issues.

```c++
EspRC.on("bedroom light", [](String state) {
  if (state.equals("on")) {
    // do something
  } else {
    // do some thing else
  }
});
EspRC.on("bedroom temp", [](Int value) {
  // Do something with int value
});
EspRC.send("bedroom light", "on");
EspRC.send("bedroom temp", 28);

```

Update 2:

1. A different way to get data. This method works better
than the previous update

```c++
EspRC.on("bedroom light", []() {
  String state = EspRC.getValue();
  if (state.equals("on")) {
    // do something
  } else {
    // do some thing else
  }
});
EspRC.on("bedroom temp", []() {
  Int value = EspRC.getValue().toInt();
  // Do something with int value
});
EspRC.send("bedroom light", "on");
EspRC.send("bedroom temp", 28);
```

## What's next

* Adding PSK encryption support
* Adding ESP32 support
* Sending typed data as argument
* Sending to a specific peer
* and much more...

## Acknowledgments

This project is part of my smarthome project that build entirely with esphome. There are more real life examples you might want to check it out at [https://github.com/iphong/ha-config](https://github.com/iphong/ha-config).

You are free to use and modify these code to better fit your project.

Cheers!!
