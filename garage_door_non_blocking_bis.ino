/*
  (c) Bill Webb 12/25/2019
  Car Garage Distance sensor
  Based on the work by Adafruit in VL53L0X Library

  Changelog

  12/25/2019 Initial release, tested on Arduino Uno only
  12/26/2019 added blink function for 'too close'
  12/27/2019 nonblocking LED added
  12/28/2019 added error test and changed LOOP delay
  12/30/2019 shut off LED at very close
  08/04/2020 shut off LED at very close not working 
             change range to int from float

*/

#include "Adafruit_VL53L0X.h"                         // Time of Flight Library
#include <jled.h>                                     // Non-blocking LED Library

#define PROGRAM "Car Garage Distance Sensor"          // Program ref
#define VERSION "08/04/2020"                          // Version ref
#define LED 13                                        // Status LED pin 13 in Builtin LED
#define DEBUG false                                   // Serial Printing
#define DELAY 60                                       // Loop delay

int range;                                          // Save range for uits conversion
bool error = true;                                    // Test for bad data

auto ledBlink = JLed(LED).Blink(300, 300).Repeat(20); // 300ms blink for 12 sec.
auto ledOn = JLed(LED).On().Blink(15000, 10);         // turn on the LED for 15 sec.
auto ledOff = JLed(LED).Off().Forever();              // Turn off the LED and leave in this default state

Adafruit_VL53L0X lox = Adafruit_VL53L0X();            // initialize


// --------------------------------------------------------- SETUP -------------------------------------
void setup() {

  Serial.begin(115200);                               // Initialize Serial for debugging

  while (! Serial) {                                 // wait until serial port opens
    delay(1);
  }

  Serial.print(PROGRAM);Serial.print("     "); Serial.println(VERSION); Serial.println();
  Serial.println("Adafruit VL53L0X test");

  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1);
  }

  Serial.println(F("VL53L0X Ranging \n\n"));          // all Okay
}

// --------------------------------------------------------- LOOP -------------------------------------
void loop() {

  VL53L0X_RangingMeasurementData_t measure;

  if (DEBUG) Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false);                 // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {                   // phase failures have incorrect data

    range = int(measure.RangeMilliMeter / 25.4);  //convert to inches from mm
    if (DEBUG) Serial.print("Distance (inches): "); if (DEBUG) Serial.println(range);
    error = false;
  } else {
    if (DEBUG) Serial.println(" out of range ");
    error = true;
  }

  if ((range > 30 && range < 40) && !error ) {                  // good spot for car

    if (DEBUG)Serial.println(" LED ON ");
    ledBlink.Reset();
    ledOn.Update();
  }

  else if ((range <=30 && range > 10) && !error ) {                          // too close

    if (DEBUG) Serial.println (" Too close - Blink");
    ledOn.Reset();
    ledBlink.Update();
  }

  else {                                            // too far - default state

    if (DEBUG) Serial.println(" LED OFF ");
    ledOff.Update();
    ledOn.Reset();
    ledBlink.Reset();
  }

  delay(DELAY);                                       // arbitrary delay to let the sensor measure
}
