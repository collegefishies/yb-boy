#ifndef graph_cpp
#define graph_cpp
#include "graph.h"

graph::graph(int x0, int y0, int x1, int y1){
		xi = x0; yi = y0; xf = x1; yf = y1;
		boundary = 10;
		numXtics = 5; numYtics = 5;
		gridSep = 2;
		numberOfTraces = 0;

		//traces[0] is the eraser array, traces[1] is the grid array.
		bookKeepers = 2;
		xtics = false;
		ytics = false;
		ylabel = false;
		xlabel = false;
		xauto = true;
		yauto = true;

		traces = new bitArray[bookKeepers];
		traces[0] = bitArray(xf-xi, yf-yi);
		traces[1] = bitArray(xf-xi, yf-yi);

		traces[0].clear();
		traces[1].clear();
}

graph::~graph(){
		delete[] traces;
}


void graph::makeAxes(){
		int x,y;

		int X0 = boundary;
		int Y0 = traces[1].C - 1 - boundary;
		int yftrial = boundary;
		int xftrial = traces[1].R - 1 - boundary;

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
				traces[1].setBit(X0,y,true);
		}

		//construct x axes;
		for (int x = X0; x <= XF; ++x)
		{
				traces[1].setBit(x,Y0,true);
		}

		if(ytics){
				//construct ytics;
				x = X0-2;
				y = Y0+2;
				int dy = (YF-Y0)/numYtics;
				for (int i = 0; i <= numYtics; ++i)
				{
						traces[1].setBit(x,Y0 + i*dy, true);
				}
		}

		if(xtics){
				//construct xtics;
				int dx = (XF-X0)/numXtics;
				for (int i = 0; i <= numXtics; ++i)
				{
						traces[1].setBit(X0+i*dx,y,true);
				}
		}
}

void graph::makeGrid(){
		//make grid 
		int x,y;

		int X0 = boundary;
		int Y0 = traces[1].C - 1 - boundary;
		int yftrial = boundary;
		int xftrial = traces[1].R - 1 - boundary;
		int YF = Y0 + ((yftrial-Y0) / numYtics)*numYtics;
		int XF = X0 + ((xftrial-X0) / numXtics)*numXtics;
		int dy = (YF-Y0)/numYtics;
		int dx = (XF-X0)/numXtics;

		if(xtics){
				for (int i = 1; i <= numXtics; i++)
				{
						for (int j = 0; j >= YF-Y0; j -= gridSep)
						{
								traces[1].setBit(X0+i*dx,Y0+j,true);
						}
				}
		}

		if(ytics){
				for (int i = 1; i <= numYtics; i++)
				{
						for (int j = 0; j <= XF-X0; j += gridSep)
						{
								traces[1].setBit(X0+j,Y0+i*dy,true);
						}
				}
		}
		
}

void graph::plotData(int traceNum, float* x, float* y, int len){
		//traceNum indexing starts at zero

		//dummy check
		if(len <= 0){
			return;
		}


		//allocate more space if needed
		if(traceNum + 1 > numberOfTraces){
			//add space for more traces!
			bitArray* tracesTemp;
			tracesTemp = new bitArray[traceNum + 1 + bookKeepers];

			for (int i = 0; i < numberOfTraces + bookKeepers; ++i)
			{
					tracesTemp[i] = traces[i];  
			}
			
			for (int i = numberOfTraces + bookKeepers; i < traceNum + 1 + bookKeepers; ++i)
			{
				tracesTemp[i] = bitArray(xf-xi,yf-yi);
			}

			delete[] traces;
			traces = tracesTemp;
			numberOfTraces = traceNum + 1;
		} else {
			//set old trace to the eraserBin!
			traces[0] = traces[0] + traces[traceNum + bookKeepers];
		}

		bitArray& graph = traces[traceNum + bookKeepers];



		//auto limit determiner
		if (xauto) {
			xmax=x[0];
			xmin=x[0];
			for (int i = 0; i < len; ++i)
			{
					xmax = max(xmax,x[i]);
					xmin = min(xmin,x[i]);
			}
		} 

		if (yauto) {
			ymax=y[0];
			ymin=y[0];
			for (int i = 0; i < len; ++i)
			{
				ymax = max(ymax,y[i]);
				ymin = min(ymin,y[i]);
			}
		}

		//figure out coordinates of data
		int intX[len];
		int intY[len];


		int X0 = boundary;
		int Y0 = graph.C - 1 - boundary;
		int yftrial = boundary;
		int xftrial = graph.R - 1 - boundary;
		int YF = Y0 + ((yftrial-Y0) / numYtics)*numYtics;
		int XF = X0 + ((xftrial-X0) / numXtics)*numXtics;
		for (int i = 0; i < len; ++i)
		{		
		 	if(x[i] > xmin && x[i] < xmax){
		 		intX[i] = round((XF-X0)*(x[i]-xmin)/(xmax-xmin)) + X0;	
		 	} else {
		 		intX[i] = -1;
		 	}
		 	
		 	if(y[i] > ymin && y[i] < ymax){
		 		intY[i] = round((YF-Y0)*(y[i]-ymin)/(ymax-ymin)) + Y0;
		 	} else {
		 		intY[i] = -1;
		 	}
		}

		for (int i = 0; i < graph.N; ++i)
		{
			graph.setBit(i,false);
		}
		for (int i = 0; i < len; ++i)
		{	
		 	graph.setBit(intX[i],intY[i],true);		
		}

}

void graph::drawGraph(){
		for (int x = 0; x < traces[1].R; ++x)
		{
				for (int y = 0; y < traces[1].C; ++y)
				{
						if(traces[1](x,y)){
								lcd.drawPixel(x+xi,y+yi,ST7735_GREEN);
						}
				}
		}
}




void graph::drawGraph(int* colors, int len){
	if( len <= 0 ){
		return;
	}

	int color = BACKGROUND;
	int draw = false;
	
	int bookkeepingColors[numberOfTraces + bookKeepers];
	bookkeepingColors[0] = BACKGROUND;

	for (int i = 0; i < min(len,numberOfTraces + bookKeepers - 1); ++i)
	{
		bookkeepingColors[i+1] = colors[i];
	}


	for (int x = 0; x < xf-xi; ++x)
	{
		for (int y = 0; y < yf-yi; ++y)
		{	
		 	draw = false;

			for(int i = 0; i < numberOfTraces + bookKeepers; i++){
				if(traces[i](x,y)){
					color = bookkeepingColors[i];
					draw = true;
				}
			}

			if(draw){
				lcd.drawPixel(x + xi, y + yi,color);
			}
			
		}
	}

	//reset eraserBin.
	for (int i = 0; i < traces[0].N; ++i)
	{
		traces[0].setBit(i,false);
	}
}
#endif
