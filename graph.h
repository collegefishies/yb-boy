#ifndef GRAPH_H
#define GRAPH_H 

#include "lcd.h"
#include "bitArray.h"
#include <math.h>

class graph {
private:
	//graphing area
	int xi,yi,xf,yf;

	//graphing options
	bool xtics,ytics,ylabel,xlabel;
	int boundary, numXtics, numYtics, gridSep;

	//graphing data
	int numberOfTraces, bookKeepers;
	int* colors;
	bitArray* traces;

	public:

		graph(int xi, int yi, int xf, int yf);
		~graph();

		//graph options
		void setXtics(int N){ xtics = N == 0 ? false : true; numXtics = N;}
		void setYtics(int N){ ytics = N == 0 ? false : true; numYtics = N;}
		void setBoundary(int N){ boundary = N;}
		void makeAxes();
		void makeGrid();

		void plotData(int traceNum, float* x, float* y,int len);
		void drawGraph();
		void drawGraph(int* colors, int len);

};




#endif
