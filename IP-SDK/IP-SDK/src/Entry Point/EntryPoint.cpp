#include <DirManager.h>
#include <opencv2/opencv.hpp>
#include "Rect.h"

template<class T>
void Thr(cv::Mat src, int value, int CentralVlaue = 0) {

	const int iPaddingWidth = 10;
	const int iPaddingHeight = 5;

	int iWidth = src.cols;
	int iHeight = src.rows;
	const int CV = CentralVlaue;

#if 0
	for (int y = iPaddingHeight; y < iHeight - iPaddingHeight; y++)
	{
		for (int x = iPaddingWidth; x < iWidth - iPaddingWidth; x++)
		{
			int val = src.at<T>(y, x);
			if (val < value) {
				src.at<T>(y, x) = 0;
			}
			else {
				src.at<T>(y, x) = 255;
			}
		}
	}
#endif

	for (int y = iPaddingHeight; y < iHeight - iPaddingHeight; y++)
	{
		for (int x = iPaddingWidth; x < iWidth - iPaddingWidth; x++)
		{
			int current = src.at<T>(y, x);
			int d = std::abs(current - CV);
			if (d < value) {
				src.at<T>(y, x) = 0;
			}
			else {
				src.at<T>(y, x) = 255;
			}
		}
	}

}

void CalcDiff(cv::Mat& src, cv::Mat& dst)
{
	int iWidth = src.cols;
	int iHeight = src.rows;
	for (int y = 0; y < iHeight; y++)
	{
		for (int x = 0; x < iWidth; x++)
		{
			int p1 = src.at<uchar>(y, x);
			int p2 = dst.at<uchar>(y, x);
			if (p1 == p2) {
				dst.at<uchar>(y, x) = 0;
			}
			else {
				dst.at<uchar>(y, x) = 255;
			}
		}
	}
}

#if 0

int main()
{
	
	pen::DirManager paths("obj_images");
	const std::vector<std::string>& all_paths = paths.GetFilesPath();

	const int x = 52;
	const int y = 50;

	for(int i = 0; i < all_paths.size() - 5; i++)
	{
		cv::Mat image = cv::imread(all_paths[i], cv::IMREAD_GRAYSCALE);
		
		cv::Mat canny;
		cv::GaussianBlur(image, image, cv::Size(5,5), 12, 2);
		//cv::boxFilter(image, image, -1, cv::Size(11, 11));
		cv::equalizeHist(image, image);

#if 1
		int centralValue = image.at<uchar>(y, x);

		Thr<uchar>(image, 20, centralValue);
		canny = image.clone();
#endif
		//Canny(image, canny, 200, 220, 3, false);
		//cv::threshold(image, canny, 0, 255, cv::THRESH_BINARY_INV);
		//adaptiveThreshold(image, canny, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 21, 2);

		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		findContours(canny, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

#if 1
		cv::RNG rng(12345);
		cv::Mat drawing = cv::Mat::zeros(canny.size(), CV_8UC3);
		for (int i = 0; i < contours.size(); i++)
		{
			cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point());
		}
#endif
		cv::imwrite("obj_res\\file_"+std::to_string(i) + ".png", image);
		//cv::namedWindow("image" + std::to_string(i), cv::WINDOW_NORMAL);
		//cv::imshow("image" + std::to_string(i), canny);
	}

	return 0;
}

#endif

int main()
{
	pen::DirManager paths("obj_images");
	const std::vector<std::string>& all_paths = paths.GetFilesPath();

	const int x = 52;
	const int y = 50;

	for (int i = 0; i < all_paths.size() - 5; i++)
	{
		cv::Mat image = cv::imread(all_paths[i], cv::IMREAD_GRAYSCALE);
		cv::Mat canny;

		cv::GaussianBlur(image, image, cv::Size(11, 11), 0.8, 12);
		//cv::boxFilter(image, image, -1, cv::Size(11, 11));

		cv::equalizeHist(image, image);

#if 1
		int centralValue = image.at<uchar>(y, x);

		Thr<uchar>(image, 25, centralValue);
		canny = image.clone();
#endif
		cv::RNG rng(12345);
		cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		cv::floodFill(canny, cv::Point(x, y), color);

		CalcDiff(canny, image);

		cv::imwrite("obj_res\\file_" + std::to_string(i) + ".png", image);
	}

	return 0;
}