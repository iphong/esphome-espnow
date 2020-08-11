//
// Author: Phong Vu
//

// THIS CODE IS OUTDATED
// CHECK OUT THE LATEST CODE AT https://github.com/iphong/lib-esp-rc

#ifndef ESP_RC_H
#define ESP_RC_H

#include <ESP8266WiFi.h>
#include <espnow.h>

#define ESP_RC_MAX_LISTENER 20
#define ESP_RC_DEBUGGER

typedef void (*esp_rc_cb_t)();
typedef void (*esp_rc_str_cb_t)(String);
typedef void (*esp_rc_int_cb_t)(int);

struct esp_rc_listener_t {
    String subscribe;
    esp_rc_cb_t callback;
    esp_rc_str_cb_t str_callback;
} listeners[ESP_RC_MAX_LISTENER];

uint8_t listeners_num = 0;

uint8_t broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

String esp_rc_msg;

void esp_rc_handle_massge(String msg) {
    for (auto i=0; i<listeners_num; i++) {
        esp_rc_listener_t listener = listeners[i];
        if (msg.startsWith(listener.subscribe)) {
            if (listener.callback) {
                listener.callback();
            }
            if (listener.str_callback) {
                String value = msg.substring(listener.subscribe.length(), msg.length());
                value.trim();
                listener.str_callback(value);
            }
        }
    }
}

uint32_t timer;

esp_now_recv_cb_t _onReceive = [](uint8_t *mac, uint8_t *payload, uint8_t size) {
    esp_rc_msg = "";
    for (auto i=0; i<size; i++) {
        esp_rc_msg.concat((const char)payload[i]);
    }
    esp_rc_handle_massge(esp_rc_msg);
    #ifdef ESP_RC_DEBUGGER
    #ifdef ESP_LOGD
        ESP_LOGD("esprc", "received: %s\n", esp_rc_msg.c_str());
    #else
        Serial.printf("received: %s\n", esp_rc_msg.c_str());
    #endif
    #endif
};

class {
public:
    void begin(uint8_t channel = 1) {
        WiFi.mode(WIFI_AP_STA);
        if (esp_now_init() == OK) {
            esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
            if (esp_now_is_peer_exist(broadcast))
                esp_now_del_peer(broadcast);
            esp_now_add_peer(broadcast, ESP_NOW_ROLE_COMBO, channel, 0, 0);
            esp_now_register_recv_cb(_onReceive);
        }
    }
    void send(String payload) {
        esp_now_send(broadcast, (uint8_t *)payload.c_str(), (uint8_t)payload.length());
    }
    void send(String payload, String value, char delimiter = ' ') {
        send(payload + delimiter + String(value));
    }
    void send(String payload, double value, char delimiter = ' ') {
        send(payload + delimiter + String(value));
    }
    void on(String  subscribe, esp_rc_cb_t callback) {
        listeners[listeners_num++] = {subscribe, callback, NULL};
    }
    void on(String  subscribe, esp_rc_str_cb_t callback) {
        listeners[listeners_num++] = {subscribe, NULL, callback};
    }
} EspRC;

#endif //ESP_RC_H

