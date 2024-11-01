// EPD_7_Colors.h
#ifndef EPD_7_COLORS_H
#define EPD_7_COLORS_H

#include <Arduino.h>
#include <SPI.h>

// IO settings
extern int BUSY_Pin;
extern int RES_Pin;
extern int DC_Pin;
extern int CS_Pin;

// 8-bit color definitions
#define Black 0x00   /// 000
#define White 0x11   /// 001
#define Green 0x22   /// 010
#define Blue 0x33    /// 011
#define Red 0x44     /// 100
#define Yellow 0x55  /// 101
#define Orange 0x66  /// 110
#define Clean 0x77   /// 111

// 4-bit color definitions
#define black 0x00   /// 000
#define white 0x01   /// 001
#define green 0x02   /// 010
#define blue 0x03    /// 011
#define red 0x04     /// 100
#define yellow 0x05  /// 101
#define orange 0x06  /// 110
#define clean 0x07   /// 111

// Macro definitions for pins
#define EPD_W21_CS_0 digitalWrite(CS_Pin, LOW)
#define EPD_W21_CS_1 digitalWrite(CS_Pin, HIGH)
#define EPD_W21_DC_0 digitalWrite(DC_Pin, LOW)
#define EPD_W21_DC_1 digitalWrite(DC_Pin, HIGH)
#define EPD_W21_RST_0 digitalWrite(RES_Pin, LOW)
#define EPD_W21_RST_1 digitalWrite(RES_Pin, HIGH)
#define isEPD_W21_BUSY digitalRead(BUSY_Pin)

// Function declarations
void driver_delay_us(unsigned int xus);
void driver_delay(unsigned long xms);
void DELAY_S(unsigned int delaytime);
void SPI_Delay(unsigned char xrate);
void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char command);
void EPD_W21_WriteCMD(unsigned char command);
void EPD_W21_Init(void);
void EPD_init(void);
void PIC_display(const unsigned char* picData);
void EPD_sleep(void);
void EPD_refresh(void);
void lcd_chkstatus(void);
void PIC_display_Clear(void);
void EPD_horizontal(void);
void EPD_vertical(void);
void Acep_color(unsigned char color);
unsigned char Color_get(unsigned char color);

#endif  // EPD_7_COLORS_H