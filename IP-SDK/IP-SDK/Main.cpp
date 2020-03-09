#include "GlobalDefines.h"
#include "Image.h"
#include <opencv2/opencv.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


using namespace sdk;

int main() 
{
	/*
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
	*/

	/*
	int iWidth, iHeight, iChannels;
	unsigned char* img = stbi_load("fbs1477_cor.tiff", &iWidth, &iHeight, &iChannels, 0);

	if (img == NULL) {
		printf("Error in loading the image\n");
	    exit(1);
	}
	printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", iWidth, iHeight, iChannels);
	*/
	
	cv::Mat img = cv::imread("test.jpg");

	cv::namedWindow("image", cv::WINDOW_NORMAL);
	cv::imshow("image", img);
	cv::waitKey(0);
	return 0;
}