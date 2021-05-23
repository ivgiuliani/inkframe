#ifndef __CONNECTIVITY_H
#define __CONNECTIVITY_H

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <esp_wifi.h>

#include "micro_utils.h"

#if !defined(WIFI_HOSTNAME)
#  error "No WIFI hostname defined"
#endif
#if !defined(WIFI_SSID)
#  error "No WIFI SSID defined"
#endif
#if !defined(WIFI_PASS)
#  error "No WIFI pass key defined"
#endif

#if !defined(WIFI_CONNECTION_TIMEOUT_MS)
#  define WIFI_CONNECTION_TIMEOUT_MS 30000
#endif

enum ConnectionInternalState {
  CI_START = 0,
  CI_CONNECTING = 1,
  CI_CONNECTED = 2,
  CI_DISCONNECTED = 3,
};

class Connectivity {
public:
  Connectivity(const uint32_t connect_timeout_ms = 30000) :
    connect_timeout_ms(connect_timeout_ms) {}

  void attempt_wifi_connection() {
    const uint32_t now = millis();

    if (std::string(WIFI_SSID).empty()) PANIC(F("Empty Wifi SSID, aborting"));
    if (std::string(WIFI_PASS).empty()) PANIC(F("Empty Wifi password, aborting"));

    Serial.printf("Attempt connection to WiFi: '%s'...\n", WIFI_SSID);

    WiFi.disconnect(false);
    WiFi.mode(WIFI_STA);
    WiFi.hostname(WIFI_HOSTNAME);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    last_connection_attempt = now;
    state = CI_CONNECTING;
  }

  void disconnect() {
    if (is_connected()) {
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
    }
  }

  ConnectionInternalState update_state() {
    const wl_status_t wifi_status = WiFi.status();

    switch (wifi_status) {
      case WL_CONNECT_FAILED:
      case WL_CONNECTION_LOST:
      case WL_NO_SSID_AVAIL:
      case WL_DISCONNECTED:
        if (connection_attempt_has_timed_out()) {
          Serial.println("Connection attempt timed out, retrying");
          attempt_wifi_connection();
        } else if (is_connected()) {
          Serial.printf("Lost connection to WIFI: %d\n", wifi_status);
          state = CI_DISCONNECTED;
        }
        break;
        if (is_connected()) {
          Serial.printf("Lost connection to WIFI: %d\n", wifi_status);
          state = CI_DISCONNECTED;
        }
        break;
      case WL_CONNECTED:
        if (!is_connected()) {
          Serial.print("Connected to WiFi, IP address: ");
          Serial.println(WiFi.localIP());

          if (!MDNS.begin(WIFI_HOSTNAME)) {
            Serial.println("Couldn't setup MDNS responder.");
          }
        }
        state = CI_CONNECTED;
        break;
      case WL_NO_SHIELD:
      case WL_IDLE_STATUS:
        if (connection_attempt_has_timed_out()) {
          Serial.println("Connection attempt timed out, retrying");
          attempt_wifi_connection();
        }
        break;
      default:
        Serial.printf("Unknown wifi state: %d\n", wifi_status);
        break;
    }

    return state;
  }

  inline bool is_connected() {
    return state == CI_CONNECTED;
  }

private:
  const uint32_t connect_timeout_ms;
  int64_t last_connection_attempt = -1;
  ConnectionInternalState state = CI_START;

  bool connection_attempt_has_timed_out() {
    const int64_t connection_elapsed = millis() - abs(last_connection_attempt);
    return state == CI_CONNECTING && connection_elapsed >= connect_timeout_ms;
  }
};

#endif // __CONNECTIVITY_H
