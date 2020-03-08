#ifndef __GLOBAL_DEFINES_H__
#define __GLOBAL_DEFINES_H__

#include <iostream>

#define DEBUG 1

#define DEBUG_PRINT(str) if(DEBUG) printf ("%s \n", str);

//void* operator new(size_t nSize)
//{
//	DEBUG_PRINT("---new---");
//	return (void*)malloc(nSize);
//}
//
//void operator delete(void* p, int iSize)
//{
//	DEBUG_PRINT("---delete---");
//	free(p);
//}


#include "TDataTypes.h"

typedef unsigned char Pixel8;
typedef unsigned short Pixel16;
typedef sdk::_data_type4<unsigned char> Pixel32;
typedef sdk::_data_type4<unsigned short> Pixel64;
typedef sdk::_data_type3<unsigned char> Pixel24;
typedef sdk::_data_type3<unsigned short> Pixel48;

#endif // !__GLOBAL_DEFINES_H__

