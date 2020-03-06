#include "GlobalDefines.h"
#include "Image.h"

using namespace sdk;

int main() 
{

	Image* img = Image::Make()
		.SetBpp(2)
		.SetComponenetCount(1)
		.SetWidth(10)
		.SetHeight(10)
		.SetZero(true)
		.Build();

	return 0;
}