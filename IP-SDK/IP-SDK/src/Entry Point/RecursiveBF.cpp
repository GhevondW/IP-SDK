#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <string.h>
#define QX_DEF_CHAR_MAX 255

struct intData4
{
	uchar r;
	uchar g;
	uchar b;
};

struct floatData3
{

    float a;
    float b;
    float c;

};

void Test() {
	float* fp = new float[15 * 3];
	float* base = fp;
	for (int i = 0; i < 15 * 3; i++)
	{
		fp[i] = i;
	}
#if 1
	for (int i = 0; i < 15; i++)
	{
		std::cout << *++fp << "," << *++fp << "," << *++fp << std::endl;
	}
#else

	floatData3* pData = (floatData3*)fp;
	for (int i = 0; i < 15; i++)
	{
		std::cout << pData[i].a << "," << pData[i].b << "," << pData[i].c << std::endl;
	}

#endif

	delete[] base;
}

inline void __Recursive_bf(cv::Vec3b* img,
							float sigma_spatial, float sigma_range,
							int width, int height, int channel,
							float* buffer = 0)
{
	const int width_height = width * height;
	const int width_channel = width * channel;
	const int width_height_channel = width * height * channel;

	bool is_buffer_internal = (buffer == 0);
	if (is_buffer_internal)
		buffer = new float[(width_height_channel + width_height
			+ width_channel + width) * 2];

	float* img_out_f = buffer;
	float* img_temp = &img_out_f[width_height_channel];
	float* map_factor_a = &img_temp[width_height_channel];
	float* map_factor_b = &map_factor_a[width_height];
	float* slice_factor_a = &map_factor_b[width_height];
	float* slice_factor_b = &slice_factor_a[width_channel];
	float* line_factor_a = &slice_factor_b[width_channel];
	float* line_factor_b = &line_factor_a[width];

	float range_table[QX_DEF_CHAR_MAX + 1];
	float inv_sigma_range = 1.0f / (sigma_range * QX_DEF_CHAR_MAX);
	for (int i = 0; i <= QX_DEF_CHAR_MAX; i++)
		range_table[i] = static_cast<float>(exp(-i * inv_sigma_range));


	float alpha = static_cast<float>(exp(-sqrt(2.0) / (sigma_spatial * width)));
	float ypr, ypg, ypb, ycr, ycg, ycb;
	float fp, fc;
	float inv_alpha_ = 1 - alpha;

    for (int y = 0; y < height; y++)
    {
		float* temp_x = &img_temp[y * width_channel];
		cv::Vec3b* in_x = &img[y * width];
		cv::Vec3b* texture_x = &img[y * width];

		cv::Vec3b in_x_pp = *in_x;
		*temp_x++ = ypr = in_x_pp[0];
		*temp_x++ = ypg = in_x_pp[1];
		*temp_x++ = ypb = in_x_pp[2];
		++in_x;

		cv::Vec3b texture_x_pp = *texture_x;
		unsigned char tpr = texture_x_pp[0];
		unsigned char tpg = texture_x_pp[1];
		unsigned char tpb = texture_x_pp[2];
		++texture_x;

		float* temp_factor_x = &map_factor_a[y * width];
		*temp_factor_x++ = fp = 1;

		// from left to right
		for (int x = 1; x < width; x++)
		{
			cv::Vec3b texture_x_pp = *texture_x;
			unsigned char tcr = texture_x_pp[0];
			unsigned char tcg = texture_x_pp[1];
			unsigned char tcb = texture_x_pp[2];
			++texture_x;

			unsigned char dr = abs(tcr - tpr);
			unsigned char dg = abs(tcg - tpg);
			unsigned char db = abs(tcb - tpb);
			int range_dist = (((dr << 1) + dg + db) >> 2);
			float weight = range_table[range_dist];
			float alpha_ = weight * alpha;

			cv::Vec3b in_x_pp = *in_x;
			*temp_x++ = ycr = inv_alpha_ * (in_x_pp[0]) + alpha_ * ypr;
			*temp_x++ = ycg = inv_alpha_ * (in_x_pp[1]) + alpha_ * ypg;
			*temp_x++ = ycb = inv_alpha_ * (in_x_pp[2]) + alpha_ * ypb;
			++in_x;
			tpr = tcr; tpg = tcg; tpb = tcb;
			ypr = ycr; ypg = ycg; ypb = ycb;
			*temp_factor_x++ = fc = inv_alpha_ + alpha_ * fp;
			fp = fc;
		}

		//Check begin
		cv::Vec3b mm_in_x = (*--in_x);
		*--temp_x; *temp_x = 0.5f * ((*temp_x) + (mm_in_x[2]));
		*--temp_x; *temp_x = 0.5f * ((*temp_x) + (mm_in_x[1]));
		*--temp_x; *temp_x = 0.5f * ((*temp_x) + (mm_in_x[0]));

		cv::Vec3b mm_texture_x = (*--texture_x);
		tpr = mm_texture_x[2];
		tpg = mm_texture_x[1];
		tpb = mm_texture_x[0];

		cv::Vec3b curr = *in_x;
		ypr = curr[2]; ypg = curr[2]; ypb = curr[2];

		*--temp_factor_x; *temp_factor_x = 0.5f * ((*temp_factor_x) + 1);
		fp = 1;

		//Check end

				// from right to left
		for (int x = width - 2; x >= 0; x--)
		{
			cv::Vec3b mm_texture_x = *--texture_x;
			unsigned char tcr = mm_texture_x[2];
			unsigned char tcg = mm_texture_x[1];
			unsigned char tcb = mm_texture_x[0];
			unsigned char dr = abs(tcr - tpr);
			unsigned char dg = abs(tcg - tpg);
			unsigned char db = abs(tcb - tpb);
			int range_dist = (((dr << 1) + dg + db) >> 2);
			float weight = range_table[range_dist];
			float alpha_ = weight * alpha;

			cv::Vec3b mm_in_x = *--in_x;
			ycr = inv_alpha_ * (mm_in_x[2]) + alpha_ * ypr;
			ycg = inv_alpha_ * (mm_in_x[1]) + alpha_ * ypg;
			ycb = inv_alpha_ * (mm_in_x[0]) + alpha_ * ypb;
			*--temp_x; *temp_x = 0.5f * ((*temp_x) + ycr);
			*--temp_x; *temp_x = 0.5f * ((*temp_x) + ycg);
			*--temp_x; *temp_x = 0.5f * ((*temp_x) + ycb);
			tpr = tcr; tpg = tcg; tpb = tcb;
			ypr = ycr; ypg = ycg; ypb = ycb;

			fc = inv_alpha_ + alpha_ * fp;
			*--temp_factor_x;
			*temp_factor_x = 0.5f * ((*temp_factor_x) + fc);
			fp = fc;
		}

    }

	alpha = static_cast<float>(exp(-sqrt(2.0) / (sigma_spatial * height)));
	inv_alpha_ = 1 - alpha;
	float* ycy, * ypy, * xcy;
	cv::Vec3b* tcy, * tpy;
	memcpy(img_out_f, img_temp, sizeof(float) * width_channel);

	float* in_factor = map_factor_a;
	float* ycf, * ypf, * xcf;
	memcpy(map_factor_b, in_factor, sizeof(float) * width);

	for (int y = 1; y < height; y++)
	{
		tpy = &img[(y - 1) * width];
		tcy = &img[y * width];
		xcy = &img_temp[y * width_channel];
		ypy = &img_out_f[(y - 1) * width_channel];
		ycy = &img_out_f[y * width_channel];

		xcf = &in_factor[y * width];
		ypf = &map_factor_b[(y - 1) * width];
		ycf = &map_factor_b[y * width];
		for (int x = 0; x < width; x++)
		{
			cv::Vec3b tcy_pp = *tcy;
			cv::Vec3b tpy_pp = *tpy;
			unsigned char dr = abs((tcy_pp[0]) - (tpy_pp[0]));
			unsigned char dg = abs((tcy_pp[1]) - (tpy_pp[1]));
			unsigned char db = abs((tcy_pp[2]) - (tpy_pp[2]));
			++tcy;
			++tpy;
			int range_dist = (((dr << 1) + dg + db) >> 2);
			float weight = range_table[range_dist];
			float alpha_ = weight * alpha;
			for (int c = 0; c < channel; c++)
				*ycy++ = inv_alpha_ * (*xcy++) + alpha_ * (*ypy++);
			*ycf++ = inv_alpha_ * (*xcf++) + alpha_ * (*ypf++);
		}
	}

	int h1 = height - 1;
	ycf = line_factor_a;
	ypf = line_factor_b;
	memcpy(ypf, &in_factor[h1 * width], sizeof(float)* width);
	for (int x = 0; x < width; x++)
		map_factor_b[h1 * width + x] = 0.5f * (map_factor_b[h1 * width + x] + ypf[x]);

	ycy = slice_factor_a;
	ypy = slice_factor_b;
	memcpy(ypy, &img_temp[h1 * width_channel], sizeof(float) * width_channel);
	int k = 0;
	for (int x = 0; x < width; x++) {
		for (int c = 0; c < channel; c++) {
			int idx = (h1 * width + x) * channel + c;
			img_out_f[idx] = 0.5f * (img_out_f[idx] + ypy[k++]) / map_factor_b[h1 * width + x];
		}
	}

	for (int y = h1 - 1; y >= 0; y--)
	{
		tpy = &img[(y + 1) * width];
		tcy = &img[y * width];
		xcy = &img_temp[y * width_channel];
		float* ycy_ = ycy;
		float* ypy_ = ypy;
		float* out_ = &img_out_f[y * width_channel];

		xcf = &in_factor[y * width];
		float* ycf_ = ycf;
		float* ypf_ = ypf;
		float* factor_ = &map_factor_b[y * width];
		for (int x = 0; x < width; x++)
		{
			cv::Vec3b tcy_pp = *tcy;
			cv::Vec3b tpy_pp = *tpy;
			unsigned char dr = abs((tcy_pp[0]) - (tpy_pp[0]));
			unsigned char dg = abs((tcy_pp[1]) - (tpy_pp[1]));
			unsigned char db = abs((tcy_pp[2]) - (tpy_pp[2]));
			++tcy;
			++tpy;
			int range_dist = (((dr << 1) + dg + db) >> 2);
			float weight = range_table[range_dist];
			float alpha_ = weight * alpha;

			float fcc = inv_alpha_ * (*xcf++) + alpha_ * (*ypf_++);
			*ycf_++ = fcc;
			*factor_ = 0.5f * (*factor_ + fcc);

			for (int c = 0; c < channel; c++)
			{
				float ycc = inv_alpha_ * (*xcy++) + alpha_ * (*ypy_++);
				*ycy_++ = ycc;
				*out_ = 0.5f * (*out_ + ycc) / (*factor_);
				*out_++;
			}
			*factor_++;
		}
		memcpy(ypy, ycy, sizeof(float) * width_channel);
		memcpy(ypf, ycf, sizeof(float) * width);
	}

	floatData3* pTmp = (floatData3*)img_out_f;
	for (int i = 0; i < width_height; ++i)
	{
		img[i][0] = static_cast<unsigned char>(pTmp[i].a);
		img[i][1] = static_cast<unsigned char>(pTmp[i].b);
		img[i][2] = static_cast<unsigned char>(pTmp[i].c);
	}

	if (is_buffer_internal)
		delete[] buffer;

}

int main() {

	std::cout << "Hello BF" << std::endl;

	cv::Mat src = cv::imread("test1.jpg");
	cv::Mat out = src.clone();


	const float sigma_range = 0.03;
	const float sigma_spatial = 0.1;

	const int width = src.cols;
	const int height = src.rows;
	const int channel = 3;

	__Recursive_bf((cv::Vec3b*)out.data, 0.2, 0.2, width, height, channel);

	cv::namedWindow("image", cv::WINDOW_NORMAL);
	cv::imshow("image", out);
	cv::waitKey(0);

	return 0;
}