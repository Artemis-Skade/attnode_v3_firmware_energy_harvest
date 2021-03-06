/*
  SG112A.h - SG112A Sensor Library
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

#ifndef SG112A_H
#define SG112A_H

#include "../../include/attsensor.h"

#define READ_TIMEOUT 500  // Timeout for Serial Communication
#define SER_BUF_LEN  16   // Length of the Internal Serial Message Buffer

#define CMD_GET_VER 0x10  // Get Sensor Version
#define CMD_GET_SER 0x12  // Get Sensor Serial
#define CMD_GET_PPM 0x14  // Get Current PPM Reading

class SG112A : public AttSensor {
  private:
    uint8_t buffer[SER_BUF_LEN];

    void write(byte cmd);
    uint8_t read();
    void zeroBuffer(void);
    uint16_t crc16(uint8_t *cmd, int len);
    uint16_t getPPM(void);

  public:
    SG112A(void);
    uint8_t getSensorData(char *payload, uint8_t startbyte);
    void calibrate(void) {};
    void initialize(void) {};
    uint8_t numBytes(void) {return 2;};
};

#endif