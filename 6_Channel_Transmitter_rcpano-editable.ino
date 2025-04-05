// 6 Channel Transmitter | 6 Kanal Verici
  #include <SPI.h>
  #include <nRF24L01.h>
  #include <RF24.h>
  const uint64_t pipeOut = 0xE9E8F0F0E1LL;   //IMPORTANT: The same as in the receiver 0xE9E8F0F0E1LL | Bu adres alıcı ile aynı olmalı
  RF24 radio(9, 10); // select CE,CSN pin | CE ve CSN pinlerin seçimi
  struct Signal {
  byte throttle;
  byte pitch;
  byte roll;
  byte yaw;
  byte aux1;
  byte aux2;
};
  Signal data;
  void ResetData() 
{
  data.throttle = 12;   // Motor stop | Motor Kapalı (Signal lost position | sinyal kesildiğindeki pozisyon)
  data.pitch = 127;    // Center | Merkez (Signal lost position | sinyal kesildiğindeki pozisyon)
  data.roll = 127;     // Center | merkez (Signal lost position | sinyal kesildiğindeki pozisyon)
  data.yaw = 127;     // Center | merkez (Signal lost position | sinyal kesildiğindeki pozisyon)
  data.aux1 = 127;    // Center | merkez (Signal lost position | sinyal kesildiğindeki pozisyon)
  data.aux2 = 127;    // Center | merkez (Signal lost position | sinyal kesildiğindeki pozisyon)
}
  void setup()
{
  //Start everything up
  radio.begin();
  radio.openWritingPipe(pipeOut);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.stopListening(); //start the radio comunication for Transmitter | Verici olarak sinyal iletişimi başlatılıyor
  ResetData();
  // Start Serial Monitor for debugging
//  Serial.begin(9600); // Initialize Serial Monitor at 9600 baud rate
 
}
  // Joystick center and its borders | Joystick merkez ve sınırları
  int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse)
{
  val = constrain(val, lower, upper);
  if ( val < middle )
  val = map(val, lower, middle, 0, 128);
  else
  val = map(val, middle, upper, 128, 255);
  return ( reverse ? 255 - val : val );
}
  void loop()
{
 

  // Setting may be required for the correct values of the control levers. | :Kontrol kolların doğru değerleri için ayar gerekebilir.
  data.throttle = mapJoystickValues( analogRead(A0), 12, 524, 1020, true );  // "true" or "false" for signal direction | "true" veya "false" sinyal yönünü belirler
  data.roll = mapJoystickValues( analogRead(A3), 12, 500, 1020, true );      // "true" or "false" for servo direction | "true" veya "false" servo yönünü belirler
  data.pitch = mapJoystickValues( analogRead(A2), 12, 517, 1020, false );     // "true" or "false" for servo direction | "true" veya "false" servo yönünü belirler
  data.yaw = mapJoystickValues( analogRead(A1), 12, 510, 1020, false );       // "true" or "false" for servo direction | "true" veya "false" servo yönünü belirler

  data.aux1 = mapJoystickValues( analogRead(A6), 12, 524, 1020, true );     // "true" or "false" for servo direction | "true" veya "false" servo yönünü belirler
  data.aux2 = mapJoystickValues( analogRead(A7), 12, 524, 1020, true );     // "true" or "false" for servo direction | "true" veya "false" servo yönünü belirler
  radio.write(&data, sizeof(Signal));

   // Print yaw, pitch, and roll values to Serial Monitor
  //Serial.print("Throttle: ");
  //Serial.print(data.throttle);
  //Serial.print("\tPitch: ");
  //Serial.print(data.pitch);
  //Serial.print("\tRoll: ");
 //Serial.print(data.roll);
 // Serial.print("\tYaw: ");
 // Serial.println(data.yaw);
  //Serial.print("\tAux1: ");
  //Serial.println(data.aux1);

  delay(100);  // Small delay before the next loop iteration to reduce serial output speed
  }
  