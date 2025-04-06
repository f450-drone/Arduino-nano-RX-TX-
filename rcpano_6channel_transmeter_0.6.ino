//6 channel tansmetter0.6
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>  // OLED display library

// OLED Definitions
#define OLED_I2C_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// NRF24 Address
const uint64_t pipeOut = 0xE9E8F0F0E1LL;  // IMPORTANT: Must match the receiver
RF24 radio(9, 10); // CE, CSN pins

// Data Structure
struct Signal {
  byte throttle;
  byte pitch;
  byte roll;
  byte yaw;
  byte aux1;
  byte aux2;
};
Signal data;
bool connectionStatus = false; // Track connection status

// OLED Initialization
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Reset Data for Signal Loss
void ResetData() {
  data.throttle = 12;  // Motor stop
  data.pitch = 127;    // Center
  data.roll = 127;
  data.yaw = 127;
  data.aux1 = 127;
  data.aux2 = 127;
}

void setup() {
//  Serial.begin(9600);  // Serial Monitor for debugging

  // OLED Initialization
  if (!display.begin(OLED_I2C_ADDRESS, OLED_RESET)) {
    Serial.println("OLED initialization failed!");
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println(F("TransmReady"));
  display.display();

  // NRF24 Setup
  radio.begin();
  radio.openWritingPipe(pipeOut);
  radio.setAutoAck(true);        // Enable acknowledgment
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.stopListening();         // Transmitter mode

  ResetData();
}

int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse) {
  val = constrain(val, lower, upper);
  if (val < middle)
    val = map(val, lower, middle, 0, 128);
  else
    val = map(val, middle, upper, 128, 255);
  return reverse ? 255 - val : val;
}
int batteryPin = A5;  // Define the analog pin where you measure the battery voltage
int batteryLevel = 0;
int batteryPercentage = 0;
void loop() {
  // Read Joystick Inputs
  data.throttle = mapJoystickValues(analogRead(A0), 12, 524, 1020, true);
  data.roll = mapJoystickValues(analogRead(A3), 12, 505, 1020, true);
  data.pitch = mapJoystickValues(analogRead(A2), 12, 515, 1020, false);
  data.yaw = mapJoystickValues(analogRead(A1), 12, 505, 1020, false);
  data.aux1 = mapJoystickValues(analogRead(A6), 12, 524, 1020, true);
  data.aux2 = mapJoystickValues(analogRead(A7), 12, 524, 1020, true);

  // Send Data and check acknowledgment
  connectionStatus = radio.write(&data, sizeof(Signal));
// Read the battery voltage
  batteryLevel = analogRead(batteryPin);  
  batteryPercentage = map(batteryLevel, 0, 1023, 0, 100);  // Map the analog value to percentage

  // Debugging Serial Output
  //Serial.print("Throttle: ");
  //Serial.print(data.throttle);
  //Serial.print("\tPitch: ");
  //Serial.print(data.pitch);
  //Serial.print("\tRoll: ");
  //Serial.print(data.roll);
  //Serial.print("\tYaw: ");
  //Serial.print(data.yaw);
  //Serial.print("\tAux1: ");
  //Serial.print(data.aux1);
  //Serial.print("\tAux2: ");
  //Serial.println(data.aux2);

  // OLED Display Update
  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("T:"); display.println(data.throttle);
  display.setCursor(40, 10);
  display.print("P:"); display.println(data.pitch);
  display.setCursor(40, 20);
  display.print("R:"); display.println(data.roll);
  display.setCursor(0, 20);
  display.print("Y:"); display.println(data.yaw);

   // Display Battery Percentage
    display.setCursor(90, 10);  // Position for Battery
    display.print("B:");
    display.print(batteryPercentage);
    display.print("%");

  // Connection Status
  display.setCursor(0, 50);
  display.println(connectionStatus ? "Rec:Connected" : "Rec:Disconnected");

  display.display();  // Show updated data
  delay(100);        // Reduce update rate
}
