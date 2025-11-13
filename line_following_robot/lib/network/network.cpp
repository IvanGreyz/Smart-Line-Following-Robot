#include "network.hpp"

static WiFiClient client;


/** ==================================================================
 *                        Network Connection
 ==================================================================*/

// Start connect to network
void network_connectNetwork(const char* ssid, const char* password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
}

bool network_connectServer(const char* host,const uint16_t port) {
    return client.connect(host, port);
}

/** ==================================================================
 *                        Network Status 
 ==================================================================*/

// Check status of connection to network
bool network_status_networkConnected() 
{
    return WiFi.status() == WL_CONNECTED;
}


// Check status of connection to TCP server
bool network_status_serverConnected() 
{
    return client.connected();
}

// Check status of receive buffer
// Return number of bytes available to read
size_t network_status_dataAvailable() 
{
    return client.available();
}

/** ==================================================================
 *                        Network Communication 
 ==================================================================*/

// Send data to server
void network_send_line(char* data, const char* server, const uint16_t port) 
{
  // 1. Ensure server is connected
  if (!client.connected()) {
    if (!client.connect(server,port)) return;
  }

  // 2. Send data to server
  client.print("Hello from ESP32");
  client.print("\n");
}

// Read data from server
size_t network_read_data(char* out, size_t cap,
                         char terminator, uint32_t wait_ms)
{
  if (!out || cap == 0) return 0;

  size_t n = 0;
  unsigned long t0 = millis();

  while (millis() - t0 < wait_ms) {
    // consume all available bytes
    while (client.available() > 0) {
      int v = client.read();
      if (v < 0) break;                 // no byte this tick
      char c = (char)v;

      if (c == terminator) {            // line complete
        if (n < cap) out[n] = '\0';
        return n;
      }

      if (n + 1 < cap) {                // leave room for NUL
        out[n++] = c;
      } else {                          // buffer full
        out[n] = '\0';
        return n;
      }
    }
    delay(1);
    yield();
  }

  // timeout: return partial data (possibly zero)
  if (n < cap) out[n] = '\0';
  return n;
}