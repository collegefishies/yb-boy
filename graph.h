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
	int numberOfTraces;
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

		void plotData(bitArray& graph, float* x, float* y,int len);
		void drawGraph();
		void drawGraph(bitArray & tr1, bitArray & tr2, int color1, int color2);
		void drawGraph(bitArray &, bitArray &, bitArray &, int color1, int color2, int color3);
    void drawGraph(bitArray* traces, int* colors, int len);

};




#endif
