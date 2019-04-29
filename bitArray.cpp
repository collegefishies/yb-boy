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

	N = cp.N;
	R = cp.R;
	C = cp.C;
	numberOfBytes = cp.numberOfBytes;	
	buff = new byte[numberOfBytes];

	if(buff == NULL){
		N = 0; R = 0; C = 0; numberOfBytes = 0; lcd.println("bitArray:Failed to Alloc");
		return;
	}

	int tx,ty;
	tx = lcd.getCursorX(); ty = lcd.getCursorY();
	for(int i = 0; i < numberOfBytes; i++){
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
	if(this != &other){
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

		for (int i = 0; i < this->numberOfBytes; ++i)
		{
			this->buff[i] = other.buff[i];
		}
	}

	return *this;
}

//move constructor
bitArray::bitArray(bitArray&& other){
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
  bitArray X = *this;
  if(numberOfBytes == other.numberOfBytes){
	  for (int i = 0; i < numberOfBytes; i++){
	    X.buff[i] = X.buff[i] | other.buff[i];
	  }
  } else {
	lcd.println("trying to add unequal bitArrays!");
  }
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

#endif
