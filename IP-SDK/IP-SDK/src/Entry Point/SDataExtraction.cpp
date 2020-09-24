
#include "Pyramid.h"

#define PI 3.14

#define __get_y(p) (p.x)
#define __get_cr(p) (p.y)
#define __get_cb(p) (p.z)

static int MAX_GRAD = 0;
static int MIN_GRAD = 0;

cv::Mat Grad(cv::Mat& image, bool gaussian = true, bool last_gaussian = true)
{
	cv::Mat ret;// = image.clone();
	//auto YCrCb = cv::cvtColor(image, cv::COLOR_BGR2YCrCb);

	//cv::resize(image, tmp, cv::Size(image.cols/2, image.rows/2));

	if (gaussian) {
		cv::GaussianBlur(image, ret, cv::Size(15, 15), 5, 5);
	}
	else {
		ret = image.clone();
	}

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



			int thr = 2;
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

	if (last_gaussian) {
		cv::GaussianBlur(ret, ret, cv::Size(11, 11), 2.5, 2.5);
	}

	for (size_t i = 1; i < h - 1; i++)
	{
		for (size_t j = 1; j < w - 1; j++)
		{
			MAX_GRAD = MAX(MAX_GRAD, ret.at<cv::Vec3b>(i, j)[0]);
			MIN_GRAD = MIN(MIN_GRAD, ret.at<cv::Vec3b>(i, j)[0]);
		}
	}

	return ret;
}


void CreateHealingMask()
{
	int begin = 1;
	int end = N - 1;

	for (int i = end; i >= begin; --i)
	{
		int current_index = i;
		int next_index = current_index - 1;

		Level& current = pyramid[current_index];
		Level& next = pyramid[next_index];

		cv::resize(current._bilateral, next._tmp, cv::Size(next._image.cols, next._image.rows));

		cv::subtract(next._tmp, next._image, next._healing);
		cv::subtract(next._image, next._tmp, next._shine);
	}

}

void ApplyMask()
{

	Level one = GetLevel(0);
	Level two = GetLevel(1);

	size_t width = one._image.cols;
	size_t height = one._image.rows;

	cv::resize(two._healing, one._tmp, cv::Size(width, height));
	cv::resize(two._shine, one._tmp2, cv::Size(width, height));

	cv::Mat origin = one._image.clone();
	cv::Mat& mask = one._tmp;
	cv::Mat& shine = one._tmp2;
	cv::Mat& ret = one._image;
	cv::Mat& grad = one._grad;

	for (size_t y = 1; y < height - 1; y++)
	{
		for (size_t x = 1; x < width - 1; x++)
		{
			if (/*shine.at<cv::Vec3b>(y,x)[0] == 0 && shine.at<cv::Vec3b>(y, x)[1] == 0 && shine.at<cv::Vec3b>(y, x)[2] == 0*/true) {
				//cv::Vec3b ret_pixel = ret.at<cv::Vec3b>(y, x);
				//cv::Vec3b mask_pixel = mask.at<cv::Vec3b>(y, x);
				//intData4 ret_ycc = RGBToYCrCb(ret_pixel[2], ret_pixel[1], ret_pixel[0]);
				//intData4 mask_ycc = RGBToYCrCb(mask_pixel[2], mask_pixel[1], mask_pixel[0]);

				//ret_ycc.y += mask_ycc.y;
				//ret_ycc.z += mask_ycc.z;

				//intData4 result_rgb = YCrCbToRGB(__get_y(ret_ycc), __get_cr(ret_ycc), __get_cb(ret_ycc), true);
#if 0
				int blend_value = grad.at<cv::Vec3b>(y, x)[0];
				blend_value += (MAX_GRAD - blend_value) / 2;
				ret.at<cv::Vec3b>(y, x)[0] = std::min(ret.at<cv::Vec3b>(y, x)[0] + mask.at<cv::Vec3b>(y, x)[0] * blend_value / MAX_GRAD, 255); //B
				ret.at<cv::Vec3b>(y, x)[1] = std::min(ret.at<cv::Vec3b>(y, x)[1] + mask.at<cv::Vec3b>(y, x)[1] * blend_value / MAX_GRAD, 255);	//G
				ret.at<cv::Vec3b>(y, x)[2] = std::min(ret.at<cv::Vec3b>(y, x)[2] + mask.at<cv::Vec3b>(y, x)[2] * blend_value / MAX_GRAD, 255);	//R
#else
				cv::Vec3b ret_pixel = ret.at<cv::Vec3b>(y, x);
				cv::Vec3b mask_pixel = mask.at<cv::Vec3b>(y, x);
				intData4 ret_ycc = RGBToYCrCb(ret_pixel[2], ret_pixel[1], ret_pixel[0]);
				intData4 mask_ycc = RGBToYCrCb(mask_pixel[2], mask_pixel[1], mask_pixel[0]);

				int blend_value = grad.at<cv::Vec3b>(y, x)[0];
				blend_value += (MAX_GRAD - blend_value) / 2;
				ret_ycc.y += mask_ycc.y * blend_value / MAX_GRAD;
				ret_ycc.z += mask_ycc.z * blend_value / MAX_GRAD;
				//ret_ycc.x = mask_ycc.x * blend_value / MAX_GRAD;

				intData4 result_rgb = YCrCbToRGB(__get_y(ret_ycc), __get_cr(ret_ycc), __get_cb(ret_ycc), true);

				
				ret.at<cv::Vec3b>(y, x)[0] = result_rgb.z; //B
				ret.at<cv::Vec3b>(y, x)[1] = result_rgb.y;	//G
				ret.at<cv::Vec3b>(y, x)[2] = result_rgb.x;	//R

#endif
			}
		}
	}

	cv::namedWindow("image_origin" + std::to_string(0), cv::WINDOW_NORMAL);
	cv::imshow("image_origin" + std::to_string(0), ret);

	cv::namedWindow("imag" + std::to_string(0), cv::WINDOW_NORMAL);
	cv::imshow("imag" + std::to_string(0), origin);

}

int main()
{
	cv::Mat image = cv::imread("C:\\Users\\Ghevond\\Desktop\\Code\\Spot.jpg");
	CreatePyramid(image);

	Iterate([](cv::Mat& image, int index) 
		{
			GetLevel(index)._grad = Grad(image, true, true);
		}, std::unique_ptr<IImageGetter>(new ImageGetter()));

	Iterate([](cv::Mat& image, int index)
		{
			cv::bilateralFilter(GetLevel(index)._image, image, 9, 50, 200);
		}, std::unique_ptr<IImageGetter>(new BilateralGetter()));

	//Iterate([](cv::Mat& image, int index)
	//	{
	//		cv::absdiff(GetLevel(index)._image, image, GetLevel(index)._healing);
	//	}, std::unique_ptr<IImageGetter>(new BilateralGetter()));

	CreateHealingMask();
	ApplyMask();

	Iterate([](cv::Mat& src, int index) {
		//if (index == 0 || index == ) return;
		cv::namedWindow("sdf" + std::to_string(index), cv::WINDOW_NORMAL);
		cv::imshow("sdf" + std::to_string(index), src);
		}, std::unique_ptr<IImageGetter>(new GradGetter()));

	//Iterate([](cv::Mat& src, int index) {
	//	if (index != 0 ) return;
	//	cv::namedWindow("image_origin" + std::to_string(index), cv::WINDOW_NORMAL);
	//	cv::imshow("image_origin" + std::to_string(index), src);
	//	}, std::unique_ptr<IImageGetter>(new HealingGetter()));

	cv::waitKey();

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