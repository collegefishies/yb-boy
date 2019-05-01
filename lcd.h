#ifndef _LCD_H_
#define _LCD_H_

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

//pinout
#define TFT_CS 0
#define TFT_DC 4
#define SD_CS  1
#define TFT_RST -1

// LCD Global Variables
#define TFT_WIDTH 80
#define CHARW   6
#define CHARH   8
#define SCR_WIDTH_IN_CHARS  25
#define SCR_HEIGHT_IN_CHARS 16
#define BACKGROUND ST7735_BLACK


class myLCD : public Adafruit_ST7735{
	using Adafruit_ST7735::Adafruit_ST7735;
	int bg = BACKGROUND;
public:
	GFXfont* getFont(){return gfxFont;};
	int getBackground(){return bg;};
	void setBackground(int color){bg = color;};
	bool getTextWrap(){return wrap;};
};

extern myLCD lcd;

void lcd_initialize(); 

void info();  //Print fun startup information

void cls();   //clear screen

void printCoords();

#endif
