#ifndef graph_cpp
#define graph_cpp
#include "graph.h"

#define BOUNDARY 10
#define OFFSET 10
#define NUMXTICS 5
#define NUMYTICS 5
#define GRIDSEP	2

graph::graph(int X, int Y){
	X0 = X;
	Y0 = Y;
}

graph::~graph(){
}

void graph::makeAxes(bitArray& graph){
	int x,y;

	int x0 = OFFSET;
	int y0 = graph.C - 1 - OFFSET;
	int yftrial = BOUNDARY;
	int xftrial = graph.R - 1 - BOUNDARY;
	int yf = y0 + ((yftrial-y0) / NUMYTICS)*NUMYTICS;
	int xf = x0 + ((xftrial-x0) / NUMXTICS)*NUMXTICS;
	
	//construct y axes;
	for (int y = y0; y >= yf ; --y)
	{
		graph.setBit(x0,y,true);
	}

	//construct x axes;
	for (int x = x0; x <= xf; ++x)
	{
		graph.setBit(x,y0,true);
	}

	//construct ytics;
	x = x0-2;
	y = y0+2;
	int dy = (yf-y0)/NUMYTICS;
	for (int i = 0; i <= NUMYTICS; ++i)
	{
		graph.setBit(x,y0 + i*dy, true);
	}

	//construct xtics;
	int dx = (xf-x0)/NUMXTICS;
	for (int i = 0; i <= NUMXTICS; ++i)
	{
		graph.setBit(x0+i*dx,y,true);
	}
}

void graph::makeGrid(bitArray & graph){
	//make grid 
	int x,y;

	int x0 = OFFSET;
	int y0 = graph.C - 1 - OFFSET;
	int yftrial = BOUNDARY;
	int xftrial = graph.R - 1 - BOUNDARY;
	int yf = y0 + ((yftrial-y0) / NUMYTICS)*NUMYTICS;
	int xf = x0 + ((xftrial-x0) / NUMXTICS)*NUMXTICS;
	int dy = (yf-y0)/NUMYTICS;
	int dx = (xf-x0)/NUMXTICS;

	for (int i = 1; i <= NUMXTICS; i++)
	{
		for (int j = 0; j >= yf-y0; j -= GRIDSEP)
		{
			graph.setBit(x0+i*dx,y0+j,true);
		}
	}

	for (int i = 1; i <= NUMYTICS; i++)
	{
		for (int j = 0; j <= xf-x0; j += GRIDSEP)
		{
			graph.setBit(x0+j,y0+i*dy,true);
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


	int x0 = OFFSET;
	int y0 = graph.C - 1 - OFFSET;
	int yftrial = BOUNDARY;
	int xftrial = graph.R - 1 - BOUNDARY;
	int yf = y0 + ((yftrial-y0) / NUMYTICS)*NUMYTICS;
	int xf = x0 + ((xftrial-x0) / NUMXTICS)*NUMXTICS;
	for (int i = 0; i < len; ++i)
	{
		xi[i] = round((xf-x0)*(x[i]-xmin)/(xmax-xmin)) + x0;
		yi[i] = round((yf-y0)*(y[i]-ymin)/(ymax-ymin)) + y0;
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

void graph::drawGraph(bitArray & graph, int color){
	for (int x = 0; x < graph.R; ++x)
	{
		for (int y = 0; y < graph.C; ++y)
		{
			if(graph(x,y)){
				lcd.drawPixel(x+X0,y+Y0,color);
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
				lcd.drawPixel(x+X0,y+Y0,color2);
			} else if(tr1(x,y)){
				lcd.drawPixel(x+X0,y+Y0,color1);
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
				lcd.drawPixel(x+X0,y+Y0,color3);
			} else if(tr2(x,y)){
				lcd.drawPixel(x+X0,y+Y0,color2);
			} else if(tr1(x,y)){
				lcd.drawPixel(x+X0,y+Y0,color1);
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
      lcd.drawPixel(x + X0, y + Y0,color);
    }
  }  
}
#endif
