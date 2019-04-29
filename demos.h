#ifndef demos_h
#define demos_h

#include <Arduino.h>

namespace Demos{

  void testGrapher();
  void testInput();
  void outputTriangleWave();
  
  void testGrapher(){

    //define graph 1
    int x0 = 0;
    int y0 = lcd.height()/2;
    int width = lcd.width();
    int height = lcd.height()/2;
    const int len = width;

  
    bitArray grid(width,height);
    bitArray trace1(width,height);
    bitArray trace1Old;
  
    graph grf(x0,y0);
    grf.makeAxes(grid);
    grf.makeGrid(grid);

    //define graph 2;
    int x10 = 0;
    int y10 = 0;

    bitArray hall(width,height);
    bitArray hallOld;
  
    graph grf1(x10,y10);

    

    float x[len];
    float y[len];
    float x1[len];
    float y1[len];

    for(int i = 0; i < len; ++i){
      x1[i] = millis();
      y1[i] = analogRead(A4);
    }

    int hallI = 0;
    int oldtime = millis();
    //calculate graph 1;  
    while(true){

      if( millis() - oldtime > 100){
        hallI = (len + hallI - 1) % len;
        oldtime = millis();
        x1[hallI] = oldtime;
        y1[hallI] = analogRead(A4);
      }
      
      for (int i = 0; i < len; ++i)
      { 
        x[i]= 6.28/len*i;
        y[i]= sin(x[i] + millis()/1000.);
      }
    
      char key = keypad.getKey();

      hallOld = hall;
      trace1Old = trace1;
      grf.plotData(trace1, x, y, len);
      grf1.plotData(hall, x1,y1,len);
      grf.drawGraph(trace1Old,grid,trace1,ST7735_BLACK,ST7735_BLUE,ST7735_YELLOW);
      grf1.drawGraph(hallOld,grid,hall,ST7735_BLACK,ST7735_BLUE,ST7735_YELLOW);
      if(!key){
        continue;
      } else {
        break;
      }
      delay(1);
    }
  }



  void testInput(){
    int num;
    lcd.setCursor(0, 0);
    num = numInput(">");
  
    lcd.println("You entered: ");
    lcd.println(num);
    wait();
  }

  void outputTriangleWave(){
    const int N = 50;
    float t[N], f[N];
    const float freq = 0.1;
    int i = 0;
    
    //define graph 1
    int x0 = 0;
    int y0 = 0;
    int width = lcd.width();
    int height = lcd.height();
    const int len = width;

  
    bitArray grid(width,height);
    bitArray trace1(width,height);
    bitArray trace1Old;
  
    graph grf(x0,y0);
    grf.makeAxes(grid);
    grf.makeGrid(grid);
      
    for( int i = 0; i < N; i++){
      t[i] = f[i] = 0;
    }

    char key = '\0';
    while(true){
      delay(1);
      i = (i + 1) % N;
      t[i] = millis()/1000.;
      f[i] = 4095*0.5*(1 + sin(2*PI*freq*t[i]));
      analogWrite(A0,round(f[i]));
      trace1Old = trace1;
      grf.plotData(trace1, t, f, N);
      grf.drawGraph(trace1Old,grid,trace1,ST7735_BLACK,ST7735_BLUE,ST7735_YELLOW);

      char key = keypad.getKey();
      
      if(!key){
        continue;
      } else {
        break;
      }
      delay(1);
    }
  }
}

#endif
