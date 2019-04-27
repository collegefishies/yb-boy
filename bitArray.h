#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H

#define DEFAULTSIZE 8

#include "input.h"


//arbitrary sized array
class bitArray {
  public:
    //byte is unsigned 8-bit type
    byte *buff;
    int N;    //number of elements
    int R;
    int C;  //lengths of row and columns
    int numberOfBytes;
  public:
    bitArray();
    bitArray(int n);
    bitArray(int r, int c);
    bitArray(const bitArray& cp);
    bitArray& operator=(const bitArray& other); //assignment constructor
    bitArray(bitArray&& other); //move constructor
    bitArray& operator=(bitArray&& other); //move assignment constructor
    
    void allocate(int r, int c);

    ~bitArray();
    
    
    bitArray operator+(const bitArray& other) const;

    //accessor
    bool operator()(int index) const;
    bool operator()(int r, int c) const;
    bool setBit(int index, bool val);
    bool setBit(int r, int c, bool val);
    bool isNull() const;
};

void testScrBitArray();

#endif
