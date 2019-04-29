/*  menu.ino --- this is a menu cpp
 *  ===============================
 *  this is intended to be the top 
 *  level file. add functions via .h
 *  files.
 * 
 *  To add your own custom function just 
 *  add a title to menuItems, increase PROGS by 1
 *  and add your void foo() function to menuProgs.
 */


//include your LCD libraries
#include "lcd.h"    //includes lcd class for screen communication
#include "input.h"  //includes keypad inputs
#include "menu.h"

#include "bitArray.h"
#include "graph.h"
#include <math.h>
#include "MemoryFree.h"

void printLabel(int x, int y, char* str, int len);


void setup() {
	lcd_initialize();
	cls();

	graph test(0,0,lcd.width(),lcd.height());
	const int N = 160;
	float x[N], y[N];
	
	for (int i = 0; i < N; ++i)
	{
		x[i] = i*10./N + millis()/1000.;
		y[i] = sin(x[i]);
	}

	test.setBoundary(20);
	test.setXtics(10);
	test.setYtics(3);
	test.makeAxes();
	test.makeGrid();
	test.setYlims(-3,3);
	test.setXlims(-10,20);

	
	// test.drawGraph();
	delay(1000);
	test.plotData(0,x,y,N);
	test.drawGraph();

	

	while(true){

		for (int i = 0; i < N; ++i)
		{
			y[i] = sin(x[i]+ millis()/1000.);
		}
		test.plotData(0,x,y,N);
		for (int i = 0; i < N; ++i)
		{
			y[i] = cos(x[i]+ millis()/1000.);
		}
		test.plotData(1,x,y,N);
		for (int i = 0; i < N; ++i)
		{
			y[i] = pow(x[i]-5.,2);
		}
		test.plotData(2,x,y,N);
		for (int i = 0; i < N; ++i)
		{
			y[i] = pow(x[i]-5 + 5*sin(millis()/500.),3);
		}
		test.plotData(3,x,y,N);
		for (int i = 0; i < N; ++i)
		{
			y[i] = exp(x[i]-5 + 5*sin(millis()/500.));
		}
		test.plotData(4,x,y,N);
		for (int i = 0; i < N; ++i)
		{
			y[i] = abs(x[i]-5 + 5*cos(millis()/500.));
		}
		test.plotData(5,x,y,N);
		for (int i = 0; i < N; ++i)
		{
			y[i] = (x[i]-5 + 5*cos(millis()/500.));
		}
		test.plotData(6,x,y,N);
		test.drawGraph();

	}
	
}


void loop() {

}


void printLabel(int x, int y, char* str, int len){
			lcd.setCursor((x+len)*CHARW,y*CHARH);
			eraseChar(len);
			lcd.print(str);
}
