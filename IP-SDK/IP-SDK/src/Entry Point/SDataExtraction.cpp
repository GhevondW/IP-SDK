
#include "Pyramid.h"

#define PI 3.14

#define __get_y(p) (p.x)
#define __get_cr(p) (p.y)
#define __get_cb(p) (p.z)


cv::Mat Grad(cv::Mat& image)
{
	cv::Mat ret;// = image.clone();
	cv::Mat tmp;
	//auto YCrCb = cv::cvtColor(image, cv::COLOR_BGR2YCrCb);

	//cv::resize(image, tmp, cv::Size(image.cols/2, image.rows/2));

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


			int gx_cr = (__get_cr(left_top) + 1 * __get_cr(left) + __get_cr(left_bottom) +
				(-1) * __get_cr(right_top) + (-1) * __get_cr(right) + (-1) * __get_cr(right_bottom));

			int gy_cr = (__get_cr(left_top) + (1) * __get_cr(top) + __get_cr(right_top) +
				(-1) * __get_cr(left_bottom) + (-1) * __get_cr(bottom) + (-1) * __get_cr(right_bottom));

			double g_cr = std::sqrt(gx_cr * gx_cr + gy_cr * gy_cr);

			int gx_cb = (__get_cb(left_top) + 1 * __get_cb(left) + __get_cb(left_bottom) +
				(-1) * __get_cb(right_top) + (-1) * __get_cb(right) + (-1) * __get_cb(right_bottom));

			int gy_cb = (__get_cb(left_top) + (1) * __get_cb(top) + __get_cb(right_top) +
				(-1) * __get_cb(left_bottom) + (-1) * __get_cb(bottom) + (-1) * __get_cb(right_bottom));

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

			int thr = 4;
			if (res_int > thr) {
				ret.at<cv::Vec3b>(i, j)[0] = res_int << 5; //B
				ret.at<cv::Vec3b>(i, j)[1] = res_int << 5;	//G
				ret.at<cv::Vec3b>(i, j)[2] = res_int << 5;	//R
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

	cv::GaussianBlur(ret, ret, cv::Size(21, 21), 5, 5);

	return ret;
}

std::pair<cv::Mat, cv::Mat> CreateHealingMask(cv::Mat& image100p)
{
	cv::Mat image50p;
	cv::Mat image25p;
	cv::Mat bilateral_50p;
	cv::Mat bilateral;
	cv::Mat mask;
	cv::Mat mask_shine;
	cv::resize(image100p, image50p, cv::Size(image100p.cols / 2, image100p.rows / 2));
	cv::resize(image50p, image25p, cv::Size(image50p.cols / 2, image50p.rows / 2));

	cv::bilateralFilter(image25p, bilateral, 9, 50, 50);

	cv::resize(bilateral, bilateral_50p, cv::Size(image50p.cols, image50p.rows));


	cv::subtract(image50p, bilateral_50p, mask_shine);
	cv::absdiff(image50p, bilateral_50p, mask);

	cv::resize(mask, mask, cv::Size(image100p.cols, image100p.rows));
	cv::resize(mask_shine, mask_shine, cv::Size(image100p.cols, image100p.rows));

	//for (size_t y = 0; y < image100p.cols; y++)
	//{
	//	for (size_t x = 0; x < image100p.rows; x++)
	//	{
	//		if (mask_shine.at<cv::Vec3b>(y, x)[0] != 0 || mask_shine.at<cv::Vec3b>(y, x)[1] != 0 || mask_shine.at<cv::Vec3b>(y, x)[2] != 0) {
	//			mask.at<cv::Vec3b>(y, x)[0] = 0;
	//			mask.at<cv::Vec3b>(y, x)[1] = 0;
	//			mask.at<cv::Vec3b>(y, x)[2] = 0;
	//		}
	//	}
	//}

	//cv::add(image100p, mask, mask);

	//cv::namedWindow("image_bliateral", cv::WINDOW_NORMAL);
	//cv::imshow("image_bliateral", mask);

	return { mask, mask_shine };
}

cv::Mat ApplyMask(cv::Mat& origin, cv::Mat& mask, cv::Mat& grad, cv::Mat& shine)
{
	size_t width = mask.cols;
	size_t height = mask.rows;
	cv::Mat ret = origin.clone();

	for (size_t y = 1; y < height - 1; y++)
	{
		for (size_t x = 1; x < width - 1; x++)
		{
			if (grad.at<cv::Vec3b>(y, x)[0] > 0) {
				ret.at<cv::Vec3b>(y, x)[0] = std::min(ret.at<cv::Vec3b>(y, x)[0] + mask.at<cv::Vec3b>(y, x)[0], 255); //B
				ret.at<cv::Vec3b>(y, x)[1] = std::min(ret.at<cv::Vec3b>(y, x)[1] + mask.at<cv::Vec3b>(y, x)[1], 255);	//G
				ret.at<cv::Vec3b>(y, x)[2] = std::min(ret.at<cv::Vec3b>(y, x)[2] + mask.at<cv::Vec3b>(y, x)[2], 255);	//R
			}
		}
	}

	return ret;
}


int main()
{
	cv::Mat image = cv::imread("C:\\Users\\Ghevond\\Desktop\\Code\\Spot_Acne2.jpg");
	CreatePyramid(image);

	return 0;
}


#if 0
int main() {	
	std::cout << "Hello World" << std::endl;

	
	
	
	auto masks = CreateHealingMask(image);
	cv::Mat grad = Grad(image);

	cv::namedWindow("image_origin", cv::WINDOW_NORMAL);
	cv::imshow("image_origin", image);

	cv::namedWindow("mask_sh", cv::WINDOW_NORMAL);
	cv::imshow("mask_sh", masks.first);

	cv::namedWindow("mask_shine", cv::WINDOW_NORMAL);
	cv::imshow("mask_shine", masks.second);

	cv::namedWindow("grad", cv::WINDOW_NORMAL);
	cv::imshow("grad", grad);

	cv::namedWindow("result", cv::WINDOW_NORMAL);
	cv::imshow("result", ApplyMask(image, masks.first, grad, masks.second));

	cv::waitKey();

	return 0;
}
#endif