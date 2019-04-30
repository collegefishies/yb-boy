#ifndef GRAPH_H
#define GRAPH_H 

#include "lcd.h"
#include "bitArray.h"
#include "Fonts/Picopixel.h"	//in Adafruit GFX library.
#include <math.h>

class graph {
private:
	//graphing area
	int xi,yi,xf,yf;

	//graphing options
	bool xtics,ytics,ylabel,xlabel,xauto,yauto;
	int boundary, numXtics, numYtics, gridSep;

	float xmax,xmin; float ymax, ymin;

	//graphing data
	int numberOfTraces;
	const int bookKeepers = 2;
	int colors[10];
	bitArray* traces;

	public:
		
		graph(int xi, int yi, int xf, int yf);
		~graph();

		//graph options
		void setXauto(){xauto = true;}
		void setYauto(){yauto = true;}
		void setAuto(){xauto = yauto = true;}
		void setXlims(float mini, float maxi){if(xmin < xmax){xauto = false; xmin = mini; xmax = maxi;}}
		void setYlims(float mini, float maxi){if(ymin < ymax){yauto = false; ymin = mini; ymax = maxi;}}
		void setXtics(int N){ xtics = N == 0 ? false : true; numXtics = N;}
		void setYtics(int N){ ytics = N == 0 ? false : true; numYtics = N;}
		void setXlabels(bool val){xlabel = val;};
		void setYlabels(bool val){ylabel = val;};
		void setColors(int* clrs, int len);
		void setBoundary(int N){ boundary = N;}

		void makeLabels();
		void makeAxes();
		void makeGrid();


		void plotData(int traceNum, float* x, float* y,int len);
		void drawGraph();
};
#endif
