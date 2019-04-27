#ifndef graph_cpp
#define graph_cpp
#include "graph.h"

graph::graph(int x0, int y0, int x1, int y1){
	xi = x0; yi = y0; xf = x1; yf = y1;
	boundary = 10;
	numXtics = 5; numYtics = 5;
	gridSep = 2;
	xtics = false;
	ytics = false;
	ylabel = false;
	xlabel = false;

	traces = new bitArray[1];
	traces[0] = bitArray(xf-xi, yf-yi);
}

graph::~graph(){
	
}


void graph::makeAxes(){
	int x,y;

	int X0 = boundary;
	int Y0 = traces[0].C - 1 - boundary;
	int yftrial = boundary;
	int xftrial = traces[0].R - 1 - boundary;

	int XF, YF;
	if(ytics){
		YF = Y0 + ((yftrial-Y0) / numYtics)*numYtics;
	} else {
		YF = yftrial;
	}

	if(xtics){
		XF = X0 + ((xftrial-X0) / numXtics)*numXtics;
	} else {
		XF = xftrial;
	}
	
	//construct y axes;
	for (int y = Y0; y >= YF ; --y)
	{
		traces[0].setBit(X0,y,true);
	}

	//construct x axes;
	for (int x = X0; x <= XF; ++x)
	{
		traces[0].setBit(x,Y0,true);
	}

	if(ytics){
		//construct ytics;
		x = X0-2;
		y = Y0+2;
		int dy = (YF-Y0)/numYtics;
		for (int i = 0; i <= numYtics; ++i)
		{
			traces[0].setBit(x,Y0 + i*dy, true);
		}
	}

	if(xtics){
		//construct xtics;
		int dx = (XF-X0)/numXtics;
		for (int i = 0; i <= numXtics; ++i)
		{
			traces[0].setBit(X0+i*dx,y,true);
		}
	}
}

void graph::makeGrid(){
	//make grid 
	int x,y;

	int X0 = boundary;
	int Y0 = traces[0].C - 1 - boundary;
	int yftrial = boundary;
	int xftrial = traces[0].R - 1 - boundary;
	int YF = Y0 + ((yftrial-Y0) / numYtics)*numYtics;
	int XF = X0 + ((xftrial-X0) / numXtics)*numXtics;
	int dy = (YF-Y0)/numYtics;
	int dx = (XF-X0)/numXtics;

	if(xtics){
		for (int i = 1; i <= numXtics; i++)
		{
			for (int j = 0; j >= YF-Y0; j -= gridSep)
			{
				traces[0].setBit(X0+i*dx,Y0+j,true);
			}
		}
	}

	if(ytics){
		for (int i = 1; i <= numYtics; i++)
		{
			for (int j = 0; j <= XF-X0; j += gridSep)
			{
				traces[0].setBit(X0+j,Y0+i*dy,true);
			}
		}
	}
	
}

void graph::plotData(bitArray & graph, float* x, float* y, int len){
	float xmax=x[0];
	float xmin=x[0];
	float ymax=y[0];
	float ymin=y[0];

	for (int i = 0; i < len; ++i)
	{
		xmax = max(xmax,x[i]);
		xmin = min(xmin,x[i]);
		ymax = max(ymax,y[i]);
		ymin = min(ymin,y[i]);
	}

	int xi[len];
	int yi[len];


	int X0 = boundary;
	int Y0 = graph.C - 1 - boundary;
	int yftrial = boundary;
	int xftrial = graph.R - 1 - boundary;
	int YF = Y0 + ((yftrial-Y0) / numYtics)*numYtics;
	int XF = X0 + ((xftrial-X0) / numXtics)*numXtics;
	for (int i = 0; i < len; ++i)
	{
		xi[i] = round((XF-X0)*(x[i]-xmin)/(xmax-xmin)) + X0;
		yi[i] = round((YF-Y0)*(y[i]-ymin)/(ymax-ymin)) + Y0;
	}

	for (int i = 0; i < graph.N; ++i)
	{
		graph.setBit(i,false);
	}
	for (int i = 0; i < len; ++i)
	{
		graph.setBit(xi[i],yi[i],true);
	}

}

void graph::drawGraph(){
	for (int x = 0; x < traces[0].R; ++x)
	{
		for (int y = 0; y < traces[0].C; ++y)
		{
			if(traces[0](x,y)){
				lcd.drawPixel(x+xi,y+yi,ST7735_GREEN);
			}
		}
	}
}

void graph::drawGraph(bitArray& tr1, bitArray& tr2, int color1, int color2){
	for (int x = 0; x < tr1.R; ++x)
	{
		for (int y = 0; y < tr1.C; ++y)
		{
			if(tr2(x,y)){
				lcd.drawPixel(x+xi,y+yi,color2);
			} else if(tr1(x,y)){
				lcd.drawPixel(x+xi,y+yi,color1);
			}
		}
	}
}

void graph::drawGraph(bitArray& tr1, bitArray& tr2, bitArray& tr3, int color1, int color2, int color3){
	for (int x = 0; x < tr1.R; ++x)
	{
		for (int y = 0; y < tr1.C; ++y)
		{
			if(tr3(x,y)){
				lcd.drawPixel(x+xi,y+yi,color3);
			} else if(tr2(x,y)){
				lcd.drawPixel(x+xi,y+yi,color2);
			} else if(tr1(x,y)){
				lcd.drawPixel(x+xi,y+yi,color1);
			}
		}
	}
}

void graph::drawGraph(bitArray* traces, int* colors, int len){
  int color = BACKGROUND;
  int draw = false;
  for (int x = 0; x < traces[0].R; ++x)
  {
    for (int y = 0; y < traces[0].C; ++y)
    {
      for(int i = 0; i < len; i++){
        if(traces[i](x,y)){
          color = colors[i];
          draw = true;
        }
      }
      lcd.drawPixel(x + xi, y + yi,color);
    }
  }  
}
#endif
