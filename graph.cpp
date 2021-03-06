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
	xtics = true;
	ytics = true;
	ylabel = true;
	xlabel = true;
	xauto = true;
	yauto = true;

	traces = new bitArray[bookKeepers];

	for (int i = 0; i < bookKeepers; ++i)
	{
		traces[i] = bitArray(xf-xi, yf-yi);
		traces[i].clear();
	}

	colors[0] = ST7735_BLACK;
	colors[1] = ST7735_BLUE;
	colors[2] = ST7735_YELLOW;
	colors[3] = ST7735_RED;
	colors[4] = ST7735_CYAN;
	colors[5] = ST77XX_MAGENTA;
	colors[6] = ST7735_ORANGE;
	colors[7] = 0xA271;	//pink
	colors[8] = 0x6352; //???
	colors[9] = 0x6472; //????
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
			int DY;
			DY = boundary-YF;
			Y0 += DY; YF += DY;
	} else {
			YF = yftrial;
	}

	if(xtics){
			XF = X0 + ((xftrial-X0) / numXtics)*numXtics;
			int DX;
			DX = traces[1].R - 1 - boundary - XF;
			X0 += DX; XF += DX;
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

void graph::makeLabels(){
	int x,y;

	int X0 = boundary;
	int Y0 = traces[1].C - 1 - boundary;
	int yftrial = boundary;
	int xftrial = traces[1].R - 1 - boundary;

	int XF, YF;
	bool wrap = lcd.getTextWrap();
	lcd.setTextWrap(false);
	int Xcursor0, Ycursor0;
	Xcursor0 = lcd.getCursorX();
	Ycursor0 = lcd.getCursorY();
	x = X0;
	y = Y0+2;



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
		

	GFXfont* initialFont = lcd.getFont();
	lcd.setFont(&Picopixel);

	if(ytics && ylabel){
			//construct y labels
			int16_t x1,y1;
			uint16_t w,h;
			String label;
			int dy = (YF-Y0)/numYtics;
			for (int i = 0; i <= numYtics; ++i)
			{
				int u=0,v=0;
				//get text size
				label = String(ymin + i*(ymax-ymin)/numYtics);
				lcd.getTextBounds(label, u, v, &x1, &y1, &w,&h);

				//shift coords according to size
				u = xi;
				v = yi + Y0 + i*dy - 1;

				//make sure you can read them.
				lcd.getTextBounds(label, u, v, &x1, &y1, &w,&h);
				lcd.fillRect(x1,y1,w,h,lcd.getBackground());
				lcd.setCursor(u,v);
				
				lcd.print(label);
			}
	}

	if(xtics && xlabel){
			int16_t x1,y1;
			uint16_t w,h;
			String label;

			//construct x labels;
			int dx = (XF-X0)/numXtics;
			for (int i = 0; i <= numXtics; ++i)
			{
				//get textsize
				int u = xi + X0+i*dx;
				int v = yi + y;
				label = String(xmin + i*(xmax-xmin)/numXtics);
				lcd.getTextBounds(label, u, v, &x1, &y1, &w,&h);
				//shift ccording to size
				v = yf - h;

				lcd.setCursor(u,v);
				//make sure you can read them
				lcd.getTextBounds(label, u, v, &x1, &y1, &w,&h);
				lcd.fillRect(x1,y1,w,h,lcd.getBackground());
				lcd.print(label);
			}
	}

	lcd.setTextWrap(wrap);
	lcd.setFont(initialFont);
	lcd.setCursor(Xcursor0, Ycursor0);
}

void graph::setColors(int* clrs, int len){
	len = min(len,10);

	for (int i = 0; i < min(len, numberOfTraces + bookKeepers - 1); ++i)
	{
		colors[i + 1] = clrs[i];
	}
	for (int i = min(len, numberOfTraces + bookKeepers - 1); i < numberOfTraces + bookKeepers - 1; ++i)
	{
		colors[i + 1] = clrs[(i % len)];	//cycle through colors if traces are left.
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

	int DX, DY;
	DY = boundary-YF;
	DX = traces[1].R - 1 - boundary - XF;

	X0 += DX; XF += DX;
	Y0 += DY; YF += DY;

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
		if(tracesTemp == NULL){
			lcd.println("graph:Failed to add trace!");
		} else {
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
		}
	} else {
		traces[0] = traces[0] + traces[traceNum + bookKeepers];
	}

	bitArray& graph = traces[traceNum + bookKeepers];


	//auto limit determiner
	if (xauto) {
		xmax=x[0];
		xmin=x[0];
		for (int i = 0; i < len; ++i)
		{	
		 	if(!isnan(x[i])){
		 		xmax = max(xmax,x[i]);
		 		xmin = min(xmin,x[i]);
		 	}
		}
	} 

	if (yauto) {
		ymax=y[0];
		ymin=y[0];
		for (int i = 0; i < len; ++i)
		{
			if(!isnan(y[i])){
				ymax = max(ymax,y[i]);
				ymin = min(ymin,y[i]);
			}
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

	int DX, DY;
	DY = boundary-YF;
	DX = graph.R - 1 - boundary - XF;

	X0 += DX; XF += DX;
	Y0 += DY; YF += DY;

	for (int i = 0; i < len; ++i)
	{		
	 	if(x[i] > xmin && x[i] < xmax && !isnan(x[i])){
	 		intX[i] = round((XF-X0)*(x[i]-xmin)/(xmax-xmin)) + X0;	
	 	} else {
	 		intX[i] = -1;
	 	}
	 	
	 	if(y[i] > ymin && y[i] < ymax && !isnan(y[i])){
	 		intY[i] = round((YF-Y0)*(y[i]-ymin)/(ymax-ymin)) + Y0;
	 	} else {
	 		intY[i] = -1;
	 	}
	}

	graph.clear();
	for (int i = 0; i < len; ++i)
	{	
	 	graph.setBit(intX[i],intY[i],true);		
	}
}

void graph::drawGraph(){
	int draw = false;
	int color;

	for (int x = 0; x < xf-xi; ++x)
	{
		for (int y = 0; y < yf-yi; ++y)
		{	
		 	draw = false;
		 	for(int i = 0; i < numberOfTraces + bookKeepers; i++){
		 		if(traces[i](x,y)){
		 			color = colors[i];
		 			draw = true;
		 		}
		 	}
		 	if(draw){lcd.drawPixel(x + xi, y + yi,color);}
		}
	}

	makeLabels();
	//reset eraserBin.
	traces[0].clear();
}

#endif
