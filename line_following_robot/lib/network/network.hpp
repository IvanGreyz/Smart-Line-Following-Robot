#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <stdint.h>
#include <stddef.h>
#include <string>

/** ==================================================================
 *                        Network Connection
 ==================================================================*/

// Start connect to network
void network_connectNetwork(const char* ssid, const char* password);
bool network_connectServer(const char* host, const uint16_t port);

/** ==================================================================
 *                        Network Status 
 ==================================================================*/

// Check status of connection to network
bool network_status_networkConnected();


// Check status of connection to TCP server
bool network_status_serverConnected();

// Check status of receive buffer
// Return number of bytes available to read
size_t network_status_dataAvailable();

/** ==================================================================
 *                        Network Communication 
 ==================================================================*/

// Send data to server
void network_send_line(char* data, const char* server, const uint16_t port);

// Read data from server
int network_read_character();
size_t network_read_data(char* out, size_t cap,
                         char terminator, uint32_t wait_ms);
