// EPD_7_Colors.cpp
#include "EPD_7_Colors.h"

// IO pin settings
int BUSY_Pin = D5;
int RES_Pin = D0;
int DC_Pin = D3;
int CS_Pin = D1;

void driver_delay_us(unsigned int xus) {
  for (; xus > 1; xus--)
    ;
}

void driver_delay(unsigned long xms) {
  unsigned long i = 0, j = 0;
  for (j = 0; j < xms; j++) {
    for (i = 0; i < 256; i++)
      ;
  }
}

void DELAY_S(unsigned int delaytime) {
  int i, j, k;
  for (i = 0; i < delaytime; i++) {
    for (j = 0; j < 4000; j++) {
      for (k = 0; k < 222; k++)
        ;
    }
  }
}

void SPI_Delay(unsigned char xrate) {
  unsigned char i;
  while (xrate) {
    for (i = 0; i < 2; i++)
      ;
    xrate--;
  }
}

void SPI_Write(unsigned char value) {
  SPI.transfer(value);
}

void EPD_W21_WriteCMD(unsigned char command) {
  SPI_Delay(1);
  EPD_W21_CS_0;
  EPD_W21_DC_0;
  SPI_Write(command);
  EPD_W21_CS_1;
}

void EPD_W21_WriteDATA(unsigned char command) {
  SPI_Delay(1);
  EPD_W21_CS_0;
  EPD_W21_DC_1;
  SPI_Write(command);
  EPD_W21_CS_1;
}

void EPD_W21_Init(void) {
  EPD_W21_RST_0;
  delay(100);
  EPD_W21_RST_1;
  delay(100);
}

void EPD_init(void) {
  EPD_W21_Init();  //Electronic paper IC reset

  EPD_W21_WriteCMD(0x01);  //POWER SETTING
  EPD_W21_WriteDATA(0x37);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x23);
  EPD_W21_WriteDATA(0x23);

  EPD_W21_WriteCMD(0X00);  //PANNEL SETTING
  EPD_W21_WriteDATA(0xEF);
  EPD_W21_WriteDATA(0x08);

  EPD_W21_WriteCMD(0x03);  //PFS
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0x06);  //boostÉè¶¨
  EPD_W21_WriteDATA(0xC7);
  EPD_W21_WriteDATA(0xC7);
  EPD_W21_WriteDATA(0x1D);


  EPD_W21_WriteCMD(0x30);   //PLL setting
  EPD_W21_WriteDATA(0x3C);  //PLL:    0-25¡æ:0x3C,25+:0x3A

  EPD_W21_WriteCMD(0X41);  //TSE
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0X50);   //VCOM AND DATA INTERVAL SETTING
  EPD_W21_WriteDATA(0x37);  //0x77

  EPD_W21_WriteCMD(0X60);  //TCON SETTING
  EPD_W21_WriteDATA(0x22);

  EPD_W21_WriteCMD(0X60);  //TCON SETTING
  EPD_W21_WriteDATA(0x22);

  EPD_W21_WriteCMD(0x61);   //tres
  EPD_W21_WriteDATA(0x02);  //source 600
  EPD_W21_WriteDATA(0x58);
  EPD_W21_WriteDATA(0x01);  //gate 448
  EPD_W21_WriteDATA(0xC0);

  EPD_W21_WriteCMD(0xE3);  //PWS
  EPD_W21_WriteDATA(0xAA);

  EPD_W21_WriteCMD(0x04);  //PWR on
  lcd_chkstatus();         //waiting for the electronic paper IC to release the idle signal
}

void EPD_refresh(void) {
  EPD_W21_WriteCMD(0x12);
  driver_delay(100);
  lcd_chkstatus();
}

void EPD_sleep(void) {
  EPD_W21_WriteCMD(0x02);
  lcd_chkstatus();
  EPD_W21_WriteCMD(0x07);
  EPD_W21_WriteDATA(0xA5);
}

void Acep_color(unsigned char color) {
  unsigned int i, j;
  EPD_W21_WriteCMD(0x10);
  for (i = 0; i < 448; i++) {
    for (j = 0; j < 300; j++) {
      EPD_W21_WriteDATA(color);
    }
  }
  EPD_W21_WriteCMD(0x12);
  delay(1);
  lcd_chkstatus();
}

unsigned char Color_get(unsigned char color) {
  unsigned char datas;
  switch (color) {
    case 0xFF: datas = white; break;
    case 0xFC: datas = yellow; break;
    case 0xEC: datas = orange; break;
    case 0xE0: datas = red; break;
    case 0x35: datas = green; break;
    case 0x2B: datas = blue; break;
    case 0x00: datas = black; break;
    default: break;
  }
  return datas;
}

void PIC_display(const unsigned char* picData) {
  unsigned int i, j, k;
  unsigned char temp1, temp2;
  unsigned char data_H, data_L, data;

  Acep_color(Clean);
  EPD_W21_WriteCMD(0x10);
  for (i = 0; i < 448; i++) {
    k = 0;
    for (j = 0; j < 300; j++) {
      temp1 = picData[i * 600 + k++];
      temp2 = picData[i * 600 + k++];
      data_H = Color_get(temp1) << 4;
      data_L = Color_get(temp2);
      data = data_H | data_L;
      EPD_W21_WriteDATA(data);
    }
  }
  EPD_W21_WriteCMD(0x12);
  delay(1);
  lcd_chkstatus();
}

void EPD_horizontal(void) {
  unsigned int i, j;
  unsigned char index = 0x00;
  unsigned char const Color[8] = { Black, White, Green, Blue, Red, Yellow, Orange, Clean };

  Acep_color(Clean);
  EPD_W21_WriteCMD(0x10);
  for (i = 0; i < 448; i++) {
    if ((i % 56 == 0) && (i != 0))
      index++;
    for (j = 0; j < 600 / 2; j++) {
      EPD_W21_WriteDATA(Color[index]);
    }
  }
  EPD_W21_WriteCMD(0x12);
  delay(1);
  lcd_chkstatus();
}

void EPD_vertical(void) {
  unsigned int i, j, k;
  unsigned char const Color[8] = { Black, White, Green, Blue, Red, Yellow, Orange, Clean };

  Acep_color(Clean);
  EPD_W21_WriteCMD(0x10);
  for (i = 0; i < 448; i++) {
    for (k = 0; k < 7; k++) {
      for (j = 0; j < 38; j++) {
        EPD_W21_WriteDATA(Color[k]);
      }
    }
    for (j = 0; j < 34; j++) {
      EPD_W21_WriteDATA(0x77);
    }
  }
  EPD_W21_WriteCMD(0x12);
  delay(1);
  lcd_chkstatus();
}

void PIC_display_Clear(void) {
  unsigned int i, j;
  Acep_color(Clean);
  EPD_W21_WriteCMD(0x10);
  for (i = 0; i < 448; i++) {
    for (j = 0; j < 300; j++) {
      EPD_W21_WriteDATA(White);
    }
  }
  EPD_W21_WriteCMD(0x12);
  delay(1);
  lcd_chkstatus();
}

void lcd_chkstatus(void) {
  while (!isEPD_W21_BUSY)
    ;
}