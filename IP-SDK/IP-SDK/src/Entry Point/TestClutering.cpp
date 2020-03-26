#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <string.h>
#include <thread>

void CalcHist(cv::Mat& src, int* const hist)
{
	int width = src.cols;
	int height = src.rows;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			++hist[src.at<uchar>(i, j)];
		}
	}

}

cv::RNG rng(12345);

int main()
{

	std::cout << "Test" << std::endl;

	cv::Mat src = cv::imread("Astro\\fbs2024_cor.tiff", cv::IMREAD_GRAYSCALE);
	cv::Mat src_tiff = cv::imread("Astro\\fbs2024_cor.tiff", -1);
	cv::Mat src_crop = src.clone();//src(cv::Rect{0,0,1500,1500});
	cv::Mat bf_crop;
	cv::Mat canny;

	cv::GaussianBlur(src_crop, src_crop, cv::Size(0, 5), 0.2, 8);
	//cv::bilateralFilter(src_crop, bf_crop, 111, 12, 4, cv::BORDER_DEFAULT);
	//cv::boxFilter(image, image, -1, cv::Size(11, 11));

	//cv::equalizeHist(src_crop, src_crop);

	//Canny(src_crop, canny, 20, 100, 3);
	adaptiveThreshold(src_crop, canny, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 99, 8);
	//cv::threshold(src_crop, canny, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	
	findContours(canny, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	//cv::threshold(src_crop, canny, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);
	
	/*
	cv::Mat drawing = cv::Mat::zeros(canny.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++)
	{
		cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point());
	}
	*/

	int w_canny = canny.cols;
	int h_canny = canny.rows;
	std::vector<cv::Rect> rects;
	double avg_height = 0;
	int count = 0;
	const int padding_x = 3;
	const int padding_y_top = 8;
	const int padding_y_bottom = 12;
	for (int i = 0; i < contours.size(); i++)
	{
		auto& points = contours[i];
		if (points.size() < 10) continue;
		auto point = points[0];
		int minx = point.x;
		int maxx = minx;
		int miny = point.y;
		int maxy = miny;
		for (int j = 1; j < points.size(); j++)
		{
			auto& p = points[j];
			int x = p.x;
			int y = p.y;
			if (x > maxx) maxx = x;
			if (x < minx) minx = x;
			if (y > maxy) maxy = y;
			if (y < miny) miny = y;
		}

		

		maxx = maxx + padding_x < w_canny ? maxx + padding_x : (w_canny - 1);
		minx = minx - padding_x >= 0 ? minx - padding_x : 0;
		maxy = maxy + padding_y_bottom < h_canny ? maxy + padding_y_bottom : (h_canny - 1);
		miny = miny - padding_y_top >= 0 ? miny - padding_y_top : 0;
		rects.push_back({minx, miny, (maxx - minx), (maxy - miny)});
		avg_height += maxy - miny;
		++count;
	}

	if (count) {
		avg_height /= count;
	}

	
	for (int i = 1; i < rects.size(); i++)
	{
		if (rects[i].height >= 50 && rects[i].height <= 140 && rects[i].width >= 5 && rects[i].width <= 50) {
			cv::Mat res = src_tiff(rects[i]);
			cv::imwrite("obj_test_res_2024\\fbs2024_" + std::to_string(i) + ".tiff", res);
		}
	}
	

	
	cv::namedWindow("image_in", cv::WINDOW_NORMAL);
	cv::imshow("image_in", canny);
	cv::waitKey(0);

	return 0;
}
