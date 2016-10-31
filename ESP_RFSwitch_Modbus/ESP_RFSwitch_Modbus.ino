// Pins used
// Temp             14 - W1
// RCSwitch         12 - analogWrite
// Opto for leds     5 - digitalWrite

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#include <ESP8266WiFi.h>
#include <Modbus.h>
#include <ModbusIP_ESP8266.h>

// Modbus Registers Offsets (0-9999)
// Coils
const int LAMP_1_COIL = 1;
const int LAMP_2_COIL = 2;
// Input
const int TEMP_REGISTER = 1;

// Pins
#define ONE_WIRE_BUS 14
#define RF_TX 12
#define LEDS_ONOFF_PIN 4

// ModbusIP object
ModbusIP mb;

// RC Switch
#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();

// W1 - Temp sensors
#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Store Temp
float t1 = 0;

// Lamp State
bool lastLamp1;
bool lastLamp2;


void setup() {
  Serial.begin(115200);
  mb.config(WIFI_SSID, WIFI_PASSWORD);
  mySwitch.enableTransmit(RF_TX);
  sensors.setResolution(11);
  
  pinMode(LEDS_ONOFF_PIN, OUTPUT);
  digitalWrite(LEDS_ONOFF_PIN, LOW);
    
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Setup Registers
  mb.addCoil(LAMP_1_COIL);
  mb.addCoil(LAMP_2_COIL);

  mb.addIreg(TEMP_REGISTER);
}
 
void loop() {
   //Call once inside loop() - all magic here
   mb.task();

   if(mb.Coil(LAMP_1_COIL) != lastLamp1) {
    Serial.print("Lamp 1 different ");
    Serial.println(mb.Coil(LAMP_1_COIL));
     if(mb.Coil(LAMP_1_COIL)) {
        mySwitch.switchOn(1, 1);
        lastLamp1 = true;
     } else {
        mySwitch.switchOff(1, 1);
        lastLamp1 = false;
     } 
   }
   

   if(mb.Coil(LAMP_2_COIL) != lastLamp2) {
    Serial.print("Lamp 2 different ");
    Serial.println(mb.Coil(LAMP_2_COIL));
     
     if(mb.Coil(LAMP_2_COIL)) {
        mySwitch.switchOn(1, 2);
        lastLamp2 = true;
     } else {
        mySwitch.switchOff(1, 2);
        lastLamp2 = false;
     } 
   }

   sensors.requestTemperatures();
   t1 = sensors.getTempCByIndex(0);
   mb.Ireg(TEMP_REGISTER, int(t1*100));
}
