#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <string.h>
#include <thread>
#include <DirManager.h>
#include <Table.h>
#include <CSVReader.h>
#include <Image.h>
#include "ColorConverterGeneric.h"

#define PI 3.14

#define __get_y(p) (p.x)
#define __get_cr(p) (p.y)
#define __get_cb(p) (p.z)

cv::Mat Grad(cv::Mat& image)
{
	cv::Mat ret = image.clone();
	//auto YCrCb = cv::cvtColor(image, cv::COLOR_BGR2YCrCb);

	cv::GaussianBlur(image, ret, cv::Size(11, 11), 5, 5);

	int w = image.cols;
	int h = image.rows;

	intData4** ycc = new intData4*[h];
	for (size_t i = 0; i < h; i++)
	{
		ycc[i] = new intData4[w];
	}

	for (size_t i = 0; i < h; i++)
	{
		for (size_t j = 0; j < w; j++)
		{
			int B = ret.at<cv::Vec3b>(i, j)[0];// = 0; //B
			int G = ret.at<cv::Vec3b>(i, j)[1];// = 255;	//G
			int R = ret.at<cv::Vec3b>(i, j)[2];// = 0;	//R

			ycc[i][j] = RGBToYCrCb(R, G, B);

		}
	}


	for (size_t i = 1; i < h - 1; i++)
	{
		for (size_t j = 1; j < w - 1; j++)
		{
			intData4& pixel = ycc[i][j];

			intData4& left			= ycc[i][j - 1];
			intData4& left_top		= ycc[i - 1][j - 1];
			intData4& left_bottom	= ycc[i + 1][j - 1];
			intData4& top			= ycc[i - 1][j];

			intData4& right			= ycc[i][j + 1];
			intData4& right_top		= ycc[i - 1][j + 1];
			intData4& right_bottom	= ycc[i + 1][j + 1];
			intData4& bottom		= ycc[i + 1][j];


			int gx_cr = (__get_cr(left_top) + 2 * __get_cr(left) + __get_cr(left_bottom) +
				(-1) * __get_cr(right_top) + (-2) * __get_cr(right) + (-1) * __get_cr(right_bottom));

			int gy_cr = (__get_cr(left_top) + (2) * __get_cr(top) + __get_cr(right_top) +
				(-1) * __get_cr(left_bottom) + (-2) * __get_cr(bottom) + (-1) * __get_cr(right_bottom));

			int g_cr = std::sqrt(gx_cr * gx_cr + gy_cr * gy_cr);

			int gx_cb = (__get_cb(left_top) + 2 * __get_cb(left) + __get_cb(left_bottom) +
				(-1) * __get_cb(right_top) + (-2) * __get_cb(right) + (-1) * __get_cb(right_bottom));

			int gy_cb = (__get_cb(left_top) + (2) * __get_cb(top) + __get_cb(right_top) +
				(-1) * __get_cb(left_bottom) + (-2) * __get_cb(bottom) + (-1) * __get_cb(right_bottom));

			double g_cb = std::sqrt(gx_cb * gx_cb + gy_cb * gy_cb);

			double theta_cr = std::atan2(gy_cr, gx_cr);// *180 / PI;
			//theta_cr = theta_cr % 180;

			double theta_cb = std::atan2(gy_cb, gx_cb);// *180 / PI;
			//theta_cb = theta_cb % 180;

			
			double xx = g_cr * std::cos(theta_cr);
			double yx = g_cb * std::cos(theta_cb);
			double xy = g_cr * std::sin(theta_cr);
			double yy = g_cb * std::sin(theta_cb);

			double res = std::sqrt((xx + yx) * (xx + yx) + (xy + yy) * (xy + yy));

			//g_cr = std::min(g_cr, 255);
			//g_cb = std::min(g_cb, 255);

			int res_int = g_cr;
			res_int = std::min(res_int, 255);

			if (res_int > 4) {
				ret.at<cv::Vec3b>(i, j)[0] = res_int; //B
				ret.at<cv::Vec3b>(i, j)[1] = res_int;	//G
				ret.at<cv::Vec3b>(i, j)[2] = res_int;	//R
			}
			else {
				ret.at<cv::Vec3b>(i, j)[0] = 0; //B
				ret.at<cv::Vec3b>(i, j)[1] = 0;	//G
				ret.at<cv::Vec3b>(i, j)[2] = 0;	//R
			}
		}
	}

	for (int i = 0; i < h; i++)
	{
		delete[] ycc[i];
	}
	delete[] ycc;


	return ret;
}

int main() {	
	std::cout << "Hello World" << std::endl;

	cv::Mat image = cv::imread("C:\\Users\\Ghevond\\Desktop\\Code\\Spot_Acne3.jpg");

	

	cv::namedWindow("image", cv::WINDOW_NORMAL);
	cv::imshow("image", Grad(image));
	cv::waitKey();
	return 0;
}