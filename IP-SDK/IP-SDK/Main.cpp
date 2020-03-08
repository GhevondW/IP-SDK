#include "GlobalDefines.h"
#include "Image.h"

using namespace sdk;

int main() 
{
	int* a = (int*)malloc(20*sizeof(int));
	Image* img = Image::Make()
		.SetBpp(2)
		.SetComponenetCount(1)
		.SetWidth(10)
		.SetHeight(10)
		.SetZero(true)
		.SetSrcData(a)
		.Wrap()
		.Build();

	delete img;
	free(a);

	return 0;
}