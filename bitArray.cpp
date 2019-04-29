#ifndef BITARRAY_CPP
#define BITARRAY_CPP

#include "bitArray.h"

//default constructor
bitArray::bitArray(){
	numberOfBytes = DEFAULTSIZE/(sizeof(byte)*8) + (DEFAULTSIZE % (sizeof(byte)*8) ? 1 : 0);
	buff = new byte [numberOfBytes];
	if (buff == NULL){
		N = 0; R = 0; C = 0; numberOfBytes = 0; lcd.println("bitArray:Failed to Alloc");
		return;
	}
	N = DEFAULTSIZE;
	R = 8;
	C = 1;
}

//1D constructor
bitArray::bitArray(int n){
	N = n;
	R = N;
	C = 1;
	numberOfBytes = n/(sizeof(byte)*8) + (n % (sizeof(byte)*8) ? 1 : 0);
	buff = new byte [numberOfBytes];
	
	if(buff == NULL){
		N = 0; R = 0; C = 0; numberOfBytes = 0; lcd.println("bitArray:Failed to Alloc");
		return;
	}

	for (int i = 0; i < numberOfBytes; ++i)
	{
		buff[i] = 0;
	}

}

//2D constructor
bitArray::bitArray(int r, int c){
	N = r*c;
	R = r;
	C = c;
	numberOfBytes = N/(sizeof(byte)*8) + (N % (sizeof(byte)*8) ? 1 : 0);
	buff = new byte [numberOfBytes];
	if(buff == NULL){
		N = 0; R = 0; C = 0; numberOfBytes = 0; lcd.println("bitArray:Failed to Alloc");
		return;
	}

	for(int i = 0; i < numberOfBytes; i++){
		buff[i] = 0;
	}
}


//copy 
bitArray::bitArray(const bitArray& cp){
	// lcd.println("In copy constructor");

	N = cp.N;
	R = cp.R;
	C = cp.C;
	// lcd.println("Bytes not equal.");
	numberOfBytes = cp.numberOfBytes;	
	buff = new byte[numberOfBytes];

	if(buff == NULL){
		N = 0; R = 0; C = 0; numberOfBytes = 0; lcd.println("bitArray:Failed to Alloc");
		return;
	}

	int tx,ty;
	tx = lcd.getCursorX(); ty = lcd.getCursorY();
	for(int i = 0; i < numberOfBytes; i++){
		// lcd.print(".");
		buff[i] = cp.buff[i];
		
	}
}


bitArray& bitArray::operator+=(const bitArray& other){
	if(numberOfBytes == other.numberOfBytes){
		for (int i = 0; i < numberOfBytes; ++i)
		{
			buff[i] |= other.buff[i];
		}
	}
	return *this;
}

void bitArray::allocate(int r, int c){
	if(buff != NULL){
		delete [] buff;	
	}
	

	N = r*c;
	R = r;
	C = c;
	numberOfBytes = N/(sizeof(byte)*8) + (N % (sizeof(byte)*8) ? 1 : 0);

	buff = new byte [numberOfBytes];

	if(buff == NULL){
		N = 0; R = 0; C = 0; numberOfBytes = 0; lcd.println("bitArray:Failed to Alloc");
		return;
	}

	for(int i = 0; i < numberOfBytes; i++){
		buff[i] = 0;
	}
}

//destructor
bitArray::~bitArray(){
	if(buff != NULL){
		delete [] buff;	
	}
	
}

bitArray& bitArray::operator=(const bitArray& other){
	// lcd.println("Assignment Constructor");
	if(this != &other){
		// char str[50];
		// sprintf(str,"A:(N,R,C,Bytes)=(%d,%d,%d,%d)",N,R,C,numberOfBytes);
		// lcd.println(str);
		// sprintf(str,"B:(N,R,C,Bytes)=(%d,%d,%d,%d)",other.N,other.R,other.C,other.numberOfBytes);
		// lcd.println(str); 
		if (this->numberOfBytes == other.numberOfBytes){
			//pass
			// lcd.println("Equal Sized!");
		} else {
			// lcd.println("Differently Sized!");
			this->numberOfBytes = other.numberOfBytes;
			delete [] this->buff;
			this->buff = new byte[numberOfBytes];

			if(this->buff == NULL){
				this->N = 0; this->R = 0; this->C = 0; this->numberOfBytes = 0; lcd.println("bitArray:Failed to Alloc");
				return *this;
			}
		}

		this->N = other.N;
		this->R = other.R;
		this->C = other.C;

		// lcd.println("Copying.");
		for (int i = 0; i < this->numberOfBytes; ++i)
		{
			this->buff[i] = other.buff[i];
		}
	}

	return *this;
}

//move constructor
bitArray::bitArray(bitArray&& other){
	// lcd.println("in move constructor");
	N = other.N;
	R = other.R;
	C = other.C;
	numberOfBytes = other.numberOfBytes;
	buff = other.buff;
	other.buff = NULL;
	other.N = 0;
	other.R = 0;
	other.C = 0;
}


//move assignment constructor
bitArray& bitArray::operator=(bitArray&& other){
	// lcd.println("In move assignment constructor");
	if (this != &other){
		if (buff != NULL)
		{
			delete[] buff;
		}
		N = other.N; R = other.R; C = other.C;
		numberOfBytes = other.numberOfBytes;
		buff = other.buff;
		other.buff = NULL;
		other.N = 0; other.R = 0; other.C = 0;
	}

	return *this;
}

const bitArray bitArray::operator+(const bitArray& other) const {
  // lcd.print("In operator+...");
  bitArray X = *this;
  if(numberOfBytes == other.numberOfBytes){
	  for (int i = 0; i < numberOfBytes; i++){
	    X.buff[i] = X.buff[i] | other.buff[i];
	  }
  } else {
	// lcd.println("trying to add unequal bitArrays!");
  }
  // lcd.println("Done!");
  return X;
}
// accessors
bool bitArray::operator()(int index) const {
	int maskBit = index % (sizeof(byte)*8);
	int arraySpot = index / (sizeof(byte)*8);
	int mask = 1 << maskBit; 
	int pulledByte = buff[arraySpot];

	return (pulledByte & mask) == mask;
}

bool bitArray::operator()(int r, int c) const {
	if (buff == NULL) {
		return false;
	}
	if (r >= R){
		return false;
	}
	if (c >= C){
		return false;
	}
	int index = c + r*C;
	int maskBit = index % (sizeof(byte)*8);
	int arraySpot = index / (sizeof(byte)*8);
	int mask = 1 << maskBit; 
	int pulledByte = buff[arraySpot];

	return pulledByte & mask;
}

bool bitArray::setBit(int index, bool val){
	if(buff == NULL){
		return !val;
	}
	if(index >= N || index < 0){
		return !val;
	}
		
	int maskBit = index % (sizeof(byte)*8);
	int arraySpot = index / (sizeof(byte)*8);
	int mask = 1 << maskBit; 
	int pulledByte = buff[arraySpot];
	int clearedByte = pulledByte - (pulledByte & mask);
	
	// int writtenByte = clearedByte + val*mask;
	if (val) {
		buff[arraySpot] = clearedByte + mask;
	} else {
		buff[arraySpot] = clearedByte;
	}
	
	return val;
}

bool bitArray::setBit(int r, int c, bool val){
	if (r >= R || r < 0){
		return !val;
	}
	if (c >= C || c < 0){
		return !val;
	}
		
	return setBit(c + r*C, val);
}

bool bitArray::isNull() const{
	if(buff == NULL){
		return true;
	} else {
		return false;  
	}
	
}

void bitArray::clear(){
	for (int i = 0; i < numberOfBytes; ++i)
	{
		buff[i] = 0;
	}
}

void testScrBitArray(){
	// test 1D array;
	cls();

	// lcd.println("Testing 1D array.");

	int onedimL = 50;
	bitArray onedim(onedimL);

	if(onedim.isNull()){
		// lcd.println("Failed to initialize bitArray.");
	}

	lcd.println("Printing all values in bitArray.");
	for(int i=0; i<onedimL; i++){
			// lcd.print(onedim(i));
	}
	// lcd.println();

	// lcd.println("Setting every third to 1.");
	for (int i = 0; i < onedimL; ++i)
	{
		if(i % 3 == 0){
			onedim.setBit(i,true);  
		} else {
			onedim.setBit(i,false);
		}
		
	}

	lcd.println("Printing all values in bitArray.");
	for(int i=0; i<onedimL; i++){
			// lcd.print(onedim(i));
	}
	// lcd.println();

	wait();

	// print diagnostic s of onedim
	cls();
	lcd.print("Null ptr="); lcd.println(int(NULL));
	lcd.print("Buff ptr="); lcd.println(int(onedim.buff));
	lcd.print("N="); lcd.println(onedim.N); 
	lcd.print("Bytes="); lcd.println(onedim.numberOfBytes);
	// lcd.println("Man'ly setting buff = 0");
	for (int i = 0; i < onedim.numberOfBytes; ++i)
	{
		onedim.buff[i] = 0;
	}
	lcd.println("Printing all values in bitArray via call");
	for(int i=0; i<onedimL; i++){
			// lcd.print(onedim(i));
	}
	// lcd.println();
	lcd.println("Printing integers stored in buff man'ly");
	for (int i = 0; i < onedim.numberOfBytes; ++i)
	{
		lcd.print(onedim.buff[i]); lcd.print(",");
	}
	// lcd.println();
	// lcd.println();
	wait();

	//testing two dim
	bitArray twodim(lcd.width(), lcd.height());

	// print diagnostics of twodim
	cls();
	if(twodim.isNull()){
		// lcd.println("Failed to initialize bitArray.");
	}
	lcd.print("Null ptr="); lcd.println(int(NULL));
	lcd.print("Buff ptr="); lcd.println(int(twodim.buff));
	lcd.print("N="); lcd.println(twodim.N); 
	lcd.print("R="); lcd.println(twodim.R);
	lcd.print("C="); lcd.println(twodim.C);
	lcd.print("RC="); lcd.println(twodim.R*twodim.C);
	lcd.print("Bytes="); lcd.println(twodim.numberOfBytes);
	wait();

	// lcd.println("Testing 1D Define");

	for (int i = 0; i < twodim.N; ++i)
	{
		twodim.setBit(i, (i%9 == 0));
	}
	lcd.println("Finished. Printing screen");
	wait();
	for (int x = 0; x < lcd.width(); ++x)
	{
		for (int y = 0; y < lcd.height(); ++y)
		{
			if(twodim(x,y)){
				lcd.drawPixel(x,y,ST7735_WHITE);  
			}
			
		}
	}
	for (int i = 0; i < twodim.N; ++i)
	{
		twodim.setBit(i, 0);
	}
	wait();

	// print test screen
	cls();
	// lcd.println("Defining Screen in 2D way");
	for (int x = 0; x < lcd.width(); ++x)
	{
		for (int y = 0; y < lcd.height(); ++y)
		{
				twodim.setBit(x,y, (x%2 == 0) && (y%2==0));
		}
	}
	wait();

	// lcd.println("Attempting Screen Draw");

	for (int x = 0; x < lcd.width(); ++x)
	{
		for (int y = 0; y < lcd.height(); ++y)
		{
			if(twodim(x,y)){
				lcd.drawPixel(x,y,ST7735_BLUE); 
			} else {
				lcd.drawPixel(x,y,ST7735_RED);
			}
			
		}
	}

	wait();
}

#endif
