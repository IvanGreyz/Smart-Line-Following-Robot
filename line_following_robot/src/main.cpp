#include <Arduino.h>
#include <HardwareSerial.h>
#include <ESP32Servo.h>
#include <Wire.h>

#include "network.hpp"

  /* ==================================================================
   *                        Definition 
   ==================================================================*/

// ========== Start robot parameters ==========
// Control parameters
static int8_t speed;
static int steeringAngle;

// Sensor signals
static uint8_t signal_ir1 = 1;
static uint8_t signal_ir2 = 1;
static uint8_t signal_ir3 = 1;
static uint8_t signal_ir4 = 1;
static uint8_t signal_ir5 = 1;
static uint16_t signal_ultrasonic1 = 0;
static uint16_t signal_ultrasonic2 = 0;
static uint16_t signal_ultrasonic3 = 0;
static int16_t signal_accel_x = 0;
static int16_t signal_accel_y = 0;
static int16_t signal_accel_z = 0;
static int16_t signal_gyro_x = 0;
static int16_t signal_gyro_y = 0;
static int16_t signal_gyro_z = 0;
volatile long signal_encoder_count = 0;
static long last_encoder_count = 0;
static float calculatedSpeed = 0;


// ========== End robot parameters ==========

// Network parameters
static const char* ssid     = "COCO 2261";      
static const char* password = "1t6#6Y11";    
static const char* server = "192.168.2.6";  
static const uint16_t port = 48561;

static char network_buffer[50];
char network_buffer_index[100];

  /* ==================================================================
   *                        Configuration 
   ==================================================================*/

// UART parameters
#define UART_RX_PIN 17
#define UART_TX_PIN 16
#define UART_BAUDRATE 115200

HardwareSerial SerialESP(2);

// Encoder parameters
#define ENCODER_A_1_PIN 18
#define ENCODER_A_2_PIN 19
#define ENCODER_B_1_PIN 23
#define ENCODER_B_1_PIN 22

// DC motor parameters
#define MOTOR_PWM_PIN 21
#define MOTOR_GATE_1_PIN 18
#define MOTOR_GATE_2_PIN 19

// Servo parameters
Servo steer;

void setup() {
  /* ==================================================================
   *                            Configuration  
   ==================================================================*/
  
  // UART configuration
  SerialESP.begin(UART_BAUDRATE, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
  Serial.begin(115200);


  // Wifi configuration
  Serial.println("Start season...");
  while(!network_status_networkConnected())
  {
    Serial.println("Connecting to WiFi...");
    network_connectNetwork(ssid, password);
    delay(1000);
  }
    while (!network_status_serverConnected()) {
    Serial.println("Connecting to Server...");
    if (!network_connectServer(server, port)) 
    { // 5s timeout
      Serial.println("TCP failed, retrying...");
    }
    delay(500);
    }
  Serial.println("Successfully connected to WiFi and Server");

  // DC motor configuration
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  pinMode(MOTOR_GATE_1_PIN, OUTPUT);
  pinMode(MOTOR_GATE_2_PIN, OUTPUT);

  // Servo configuration
  steer.attach(15);  // Attach servo to GPIO 22
}

void loop() 
{
  /* ==================================================================
   *                        Sensor Signal Acquisition 
   ==================================================================*/

    // UART communication to read sensor data
      // 1. Send request to sensor node
    SerialESP.print("G\n");    

    // 2. Read the response with timeout
    if (SerialESP.available()) {
      // Read incoming data
      SerialESP.setTimeout(50);
      String sensorData = SerialESP.readStringUntil('\n');
      // Serial.print("Received from UART: ");
      Serial.println(sensorData);
        
      // Parse sensor data (assuming CSV format: ir1,ir2,ir3,ir4,ir5,ultrasonic1,ultrasonic2,ultrasonic3,accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,encoder_count)
      sscanf(sensorData.c_str(), "%d,%d,%d,%d,%d,%.3f,%.3f,%.3f,%d,%d,%d,%d,%d,%d,%ld",
            &signal_ir1, &signal_ir2, &signal_ir3, &signal_ir4, &signal_ir5,
            &signal_ultrasonic1, &signal_ultrasonic2, &signal_ultrasonic3,
            &signal_accel_x, &signal_accel_y, &signal_accel_z,
            &signal_gyro_x, &signal_gyro_y, &signal_gyro_z);
        
      // Debug print parsed values
              // Serial.print("IR Signals: ");
              // Serial.print(signal_ir1); Serial.print(", ");
              // Serial.print(signal_ir2); Serial.print(", ");
              // Serial.print(signal_ir3); Serial.print(", ");
              // Serial.print(signal_ir4); Serial.print(", ");
              // Serial.println(signal_ir5);
                
              // Serial.print("Ultrasonic Signals: ");
              // Serial.print(signal_ultrasonic1); Serial.print(", ");
              // Serial.print(signal_ultrasonic2); Serial.print(", ");
              // Serial.println(signal_ultrasonic3);
    }
    // Read encoders




    /* ==================================================================
    *                        Network communication 
    ==================================================================*/
    // Send signal to server
    Serial.println("\nSending data to server");
    
    int n = snprintf(network_buffer_index, sizeof(network_buffer_index),
      "%d %d %d %d %d %.3f %.3f %.3f %d %d %d %d %d %d %d",
      signal_ir1, signal_ir2, signal_ir3, signal_ir4, signal_ir5,
      signal_ultrasonic1, signal_ultrasonic2, signal_ultrasonic3,
      signal_accel_x, signal_accel_y, signal_accel_z,
      signal_gyro_x, signal_gyro_y, signal_gyro_z,signal_encoder_count);
    
    if((size_t)n >= sizeof(network_buffer_index)) // If the buffer is overflowed
    { 
      network_send_line(network_buffer_index, server,port);
    }
    else
    {
      Serial.println("Network buffer overflow, data not sent");
    }

    // Read commands from server
    if(network_status_dataAvailable())
    {
      if (network_read_data(network_buffer, sizeof(network_buffer), '\n', 10) > 0) 
      {
        Serial.println("Data available from server");
        // Process received data and update control parameters
        Serial.print("Received from server: ");
        

        // Extract data from data frame buffer
        // | 1 byte |     4 bytes     |
        // | speed  |  steeringAngle  |
        // Bytes are little-endian int32 then 1-byte trailer:
        speed = (int8_t)network_buffer[0];
        memcpy(&steeringAngle, network_buffer+1, 2);        // little-endian on ESP32 too
        
        Serial.printf("speed=%ld, trailer=%u\n",speed, steeringAngle);
        Serial.print("Updated speed: ");
        Serial.println(speed);
        Serial.print("Updated steeringAngle: ");
        Serial.println(steeringAngle);
      }
    }
    

    /* ==================================================================
    *                        Control Actuators 
    ==================================================================*/

    // Control DC motors
    // Map speed (-128 to 127) to PWM value (0 to 255)
    analogWrite(MOTOR_PWM_PIN, map(abs(speed), -128, 127, 0, 255));
    if (speed > 0) {
      digitalWrite(MOTOR_GATE_1_PIN, HIGH);
      digitalWrite(MOTOR_GATE_2_PIN, LOW);
    } else if (speed < 0) {
      digitalWrite(MOTOR_GATE_1_PIN, LOW);
      digitalWrite(MOTOR_GATE_2_PIN, HIGH);
    } else {
      digitalWrite(MOTOR_GATE_1_PIN, LOW);
      digitalWrite(MOTOR_GATE_2_PIN, LOW);
    }

    // Control servo
    int val = map(steeringAngle, 0, 1023, 0, 180);
    steer.write(val);
}