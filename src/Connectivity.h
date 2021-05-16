#ifndef __CONNECTIVITY_H
#define __CONNECTIVITY_H

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <esp_wifi.h>

#define Q(x) #x
#define QUOTE(x) Q(x)

#define WIFI_HOSTNAME QUOTE(_WIFI_HOSTNAME)
#ifndef _WIFI_SSID
#  error "No WIFI SSID defined"
#else
#  define WIFI_SSID QUOTE(_WIFI_SSID)
#endif

#ifndef _WIFI_PASS
#  error "No WIFI pass key defined"
#else
#  define WIFI_PASS QUOTE(_WIFI_PASS)
#endif

class Connectivity {
public:
  void attempt_wifi_connection(uint32_t next_attempt_delay_ms = 30000) {
    const uint32_t now = millis();

    if (std::string(WIFI_SSID).empty()) PANIC(F("Empty Wifi SSID, aborting"));
    if (std::string(WIFI_PASS).empty()) PANIC(F("Empty Wifi password, aborting"));

    if (last_connection_attempt < 0 || now - last_connection_attempt >= next_attempt_delay_ms) {
      Serial.printf("Attempt connection to WiFi: '%s'...\n", WIFI_SSID);

      WiFi.disconnect(false);
      WiFi.mode(WIFI_STA);
      WiFi.hostname(WIFI_HOSTNAME);
      WiFi.begin(WIFI_SSID, WIFI_PASS);

      last_connection_attempt = now;
    }
  }

  void disconnect() {
    if (connected) {
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
    }
  }

  void update_state() {
    const wl_status_t wifi_status = WiFi.status();

    switch (wifi_status) {
      case WL_CONNECT_FAILED:
      case WL_CONNECTION_LOST:
      case WL_DISCONNECTED:
      case WL_NO_SSID_AVAIL:
        if (connected) {
          Serial.printf("Lost connection to WIFI: %d\n", wifi_status);
          connected = false;
        }
        break;
      case WL_CONNECTED:
        if (!connected) {
          Serial.print("Connected to WiFi, IP address: ");
          Serial.println(WiFi.localIP());

          if (!MDNS.begin(WIFI_HOSTNAME)) {
            Serial.println("Couldn't setup MDNS responder.");
          }
        }
        connected = true;
        return;
      case WL_NO_SHIELD:
      case WL_IDLE_STATUS:
        // Nothing to do in these cases.
        break;
      default:
        Serial.printf("Unknown wifi state: %d\n", wifi_status);
        break;
    }
  }

  bool is_connected() {
    return connected;
  }

private:
  long last_connection_attempt = -1;
  bool connected = false;
};

#endif // __CONNECTIVITY_H
