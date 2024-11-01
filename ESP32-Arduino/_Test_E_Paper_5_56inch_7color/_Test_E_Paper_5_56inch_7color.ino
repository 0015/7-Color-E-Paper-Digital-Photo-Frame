/*
  ESP32 E-Paper Digital Frame Project
  https://github.com/0015/7-Color-E-Paper-Digital-Photo-Frame

  5.65" Seven-Color eInk
  https://www.seeedstudio.com/5-65-Seven-Color-ePaper-Display-with-600x480-Pixels-p-5786.html

  XIAO eInk Expansion Board
  https://wiki.seeedstudio.com/XIAO-eInk-Expansion-Board/

  XIAO ESP32-S3
  https://www.seeedstudio.com/XIAO-ESP32S3-p-5627.html

  ESP32 Arduino Core
  Version: 3.0.7
*/

#include "EPD_7_Colors.h"
#include "demo_image.h"

void setup() {
  Serial.begin(115200);

  pinMode(BUSY_Pin, INPUT);
  pinMode(RES_Pin, OUTPUT);
  pinMode(DC_Pin, OUTPUT);
  pinMode(CS_Pin, OUTPUT);

  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  SPI.begin();

  EPD_init();   
  PIC_display(demo_image);
  EPD_sleep(); 
}

void loop() {
  delay(60000);
}
