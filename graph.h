#ifndef GRAPH_H
#define GRAPH_H

#include "lcd.h"
#include "bitArray.h"
#include <math.h>

// to graph:
//	* need to plot grid
//	* need to plot data
//	*	done!
class graph {
	public:
		int X0;
		int Y0;
		int numberOfTraces;

		bitArray* trace;

		graph(int X0, int Y0);
		~graph();

		void makeAxes(bitArray& graph);
		void makeGrid(bitArray& graph);
		void plotData(bitArray& graph, float* x, float* y,int len);
		void drawGraph(bitArray & graph, int color);
		void drawGraph(bitArray & tr1, bitArray & tr2, int color1, int color2);
		void drawGraph(bitArray &, bitArray &, bitArray &, int color1, int color2, int color3);
    void drawGraph(bitArray* traces, int* colors, int len);

};

#endif
