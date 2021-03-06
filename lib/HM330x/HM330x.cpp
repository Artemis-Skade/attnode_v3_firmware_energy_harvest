/*
  HM330x.cpp - Seeed HM330x Particle-Sensor Library
  Copyright (c) 2020-2021, Stefan Brand
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  1. Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
  3. Neither the name of the copyright holder nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <Arduino.h>
#include <inttypes.h>
#include <Wire.h>
#include "HM330x.h"

// Default Constructor
HM330x::HM330x(uint8_t sp = 0) {
  sleep_pin = sp;
};

// Initialize the Sensor
void HM330x::initialize(void) {
  uint8_t retryCount = 0;
  DEBUG_PRINTLN("HM330x::initialize");
  
  // Enable Sleep Mode if Pin is Configured
  if (sleep_pin > 0) {
    pinMode(sleep_pin, OUTPUT);
    digitalWrite(sleep_pin, HIGH);
  }

  // Wait for Sensor to get Ready
  if (sleep_pin == 0) {
    DEBUG_PRINTLN("HM330x::initialize Waiting for Sensor Startup");
    delay(30000);
  }


  DEBUG_PRINTLN("HM330x::initialize Trying to send Select");
  // Send select command
  while (!sendCmd(HM330x_SELECT)) {
    if (retryCount++ >= 10) {
      DEBUG_PRINTLN("HM330x::initialize Select Failed!");
      return;
    } else {
      delay(500);
      DEBUG_PRINTLN("HM330x::initialize Retrying Select...");
    }
  }
}

// Read Data From Sensor and Put Them Into the Payload Array
uint8_t HM330x::getSensorData(char *payload, uint8_t startbyte) {
  uint8_t data[HM330x_DATA_LEN] = {0};
  uint16_t values[3] = { 0xEEEE, 0xEEEE, 0xEEEE };

  DEBUG_PRINTLN("HM330x::getSensorData");
  
  // Enable Sensor and Wait for it to Settle
  if (sleep_pin > 0) {
    digitalWrite(sleep_pin, HIGH);
    DEBUG_PRINTLN("HM330x::getSensorData Waiting for Stable Values after Sleep");
    delay(30000);
    sendCmd(HM330x_SELECT);
  }

  // Initialize Payload with 0s
  for (uint8_t i=startbyte; i < startbyte+6; i++)
    payload[i] = 0xFF;

  bool    dataok = false;
  uint8_t tries  = 5;

  while (!dataok && tries > 0) {
    DEBUG_PRINT("HM330x::getSensorData reading I2C Try ");
    DEBUG_PRINTLN(6-tries);
    // Get Data from the Sensors
    Wire.requestFrom(HM330x_ADDR, HM330x_DATA_LEN);
    if (Wire.available()) {
      DEBUG_PRINT("HM330x::getSensorData I2C data: 0x")
      for (uint8_t i = 0; i<HM330x_DATA_LEN; i++){
        data[i] = Wire.read();
        DEBUG_PRINT(data[i], HEX);
        DEBUG_PRINT(" ");
      }
      DEBUG_PRINTLN("");

      if (calcSum(data) == data[HM330x_DATA_LEN-1]) {
        for (uint8_t pos = 5; pos<8; pos++) {
          values[pos-5]= bytesToUint16(data, pos);
        }
        dataok = true;
      } else {
        DEBUG_PRINTLN("HM330x::getSensorData Checksum Error")
        delay(2000);
        tries--;
      }
    } else {
      DEBUG_PRINTLN("HM330x::getSensorData I2C Not Ready")
      delay(2000);
      tries--;
    }
  }
  uint16ToPayload(values[0], payload, startbyte);
  uint16ToPayload(values[1], payload, startbyte+2);
  uint16ToPayload(values[2], payload, startbyte+4);
  if (sleep_pin > 0) {
    digitalWrite(sleep_pin, LOW);
  }
  return startbyte+6;
}

// Send only a Command
bool HM330x::sendCmd(uint8_t cmd) {
  DEBUG_PRINT("HM330x::sendCmd: 0x")
  DEBUG_PRINTLN(cmd, HEX);
  Wire.beginTransmission(HM330x_ADDR);
  Wire.write(cmd);
  return Wire.endTransmission();
}

// Calculate the CheckSum
uint8_t HM330x::calcSum(uint8_t bytes[]) {
  uint8_t checksum = 0;
  for (int i = 0; i < HM330x_DATA_LEN - 1; i++) {
    checksum += bytes[i];
  }
  DEBUG_PRINT("HM330x::calcSum: 0x");
  DEBUG_PRINTLN(checksum, HEX);
  return checksum;
}

// Get Value from Sensor Bytes Array
uint16_t HM330x::bytesToUint16(uint8_t bytes[], uint8_t pos){
  return bytes[pos * 2] * 0x100 + bytes[pos * 2 + 1];
}