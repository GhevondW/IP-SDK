#if 0

#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <string.h>
#include <thread>
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

	float& operator[](int index) {
		if (index == 0)
		{
			return a;
		}
		else if (index == 1) {
			return b;
		}
		else if (index == 2) {
			return c;
		}
		else
		{
			throw "invalid oeration";
		}
	}

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

inline void __Recursive_bf_rgb(cv::Vec3b* img, cv::Vec3b* out,
							float sigma_spatial, float sigma_range,
							int width, int height, int channel,
							float* buffer, const float* const range_table)
{
	const int width_height = width * height;
	const int width_channel = width * channel;
	const int width_height_channel = width * height * channel;

	float* img_out_f = buffer;
	float* img_temp = &img_out_f[width_height_channel];
	float* map_factor_a = &img_temp[width_height_channel];
	float* map_factor_b = &map_factor_a[width_height];
	float* slice_factor_a = &map_factor_b[width_height];
	float* slice_factor_b = &slice_factor_a[width_channel];
	float* line_factor_a = &slice_factor_b[width_channel];
	float* line_factor_b = &line_factor_a[width];

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

		if (y == height - 1) {
			for (int x = 0; x < width; x++)
			{
				float* in_factor = map_factor_a;
				int dp_tmp = x * channel;
				img_out_f[dp_tmp] = img_temp[dp_tmp];
				img_out_f[dp_tmp + 1] = img_temp[dp_tmp + 1];
				img_out_f[dp_tmp + 2] = img_temp[dp_tmp + 2];

				map_factor_b[x] = in_factor[x];
			}
		}

    }

	alpha = static_cast<float>(exp(-sqrt(2.0) / (sigma_spatial * height)));
	inv_alpha_ = 1 - alpha;
	float* ycy, * ypy, * xcy;
	cv::Vec3b* tcy, * tpy;
	//memcpy(img_out_f, img_temp, sizeof(float) * width_channel);

	float* in_factor = map_factor_a;
	float* ycf, * ypf, * xcf;
	//memcpy(map_factor_b, in_factor, sizeof(float) * width);

	int h1 = height - 1;
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

			if (y == h1) {
				ypf = line_factor_b;
				ypf[x] = in_factor[h1 * width + x];
				map_factor_b[h1 * width + x] = 0.5f * (map_factor_b[h1 * width + x] + ypf[x]);

				ycy = slice_factor_a;
				ypy = slice_factor_b;
#if 0
				memcpy(ypy, &img_temp[h1 * width_channel], sizeof(float)* width_channel);
#else
				int tmp_dp = x * channel;
				ypy[tmp_dp] = img_temp[h1 * width_channel + tmp_dp];
				ypy[tmp_dp + 1] = img_temp[h1 * width_channel + tmp_dp + 1];
				ypy[tmp_dp + 2] = img_temp[h1 * width_channel + tmp_dp + 2];
#endif
				int k = 0;
				for (int x = 0; x < width; x++) {
					for (int c = 0; c < channel; c++) {
						int idx = (h1 * width + x) * channel + c;
						img_out_f[idx] = 0.5f * (img_out_f[idx] + ypy[k++]) / map_factor_b[h1 * width + x];
					}
				}
			}

		}
	}
#if 0
	int h1 = height - 1;
#endif
	ycf = line_factor_a;
	ypf = line_factor_b;
#if 0
	memcpy(ypf, &in_factor[h1 * width], sizeof(float)* width);
	for (int x = 0; x < width; x++)
	map_factor_b[h1 * width + x] = 0.5f * (map_factor_b[h1 * width + x] + ypf[x]);
#endif
	ycy = slice_factor_a;
	ypy = slice_factor_b;
#if 0
	memcpy(ypy, &img_temp[h1 * width_channel], sizeof(float) * width_channel);
	int k = 0;
	for (int x = 0; x < width; x++) {
		for (int c = 0; c < channel; c++) {
			int idx = (h1 * width + x) * channel + c;
			img_out_f[idx] = 0.5f * (img_out_f[idx] + ypy[k++]) / map_factor_b[h1 * width + x];
		}
	}
#endif
	floatData3* pTmp = (floatData3*)img_out_f;
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

			int dp = y * width + x;
			out[dp][0] = static_cast<unsigned char>(pTmp[dp].a);
			out[dp][1] = static_cast<unsigned char>(pTmp[dp].b);
			out[dp][2] = static_cast<unsigned char>(pTmp[dp].c);


			int dp_ypy = x * channel;
			ypy[dp_ypy] = ycy[dp_ypy];
			ypy[dp_ypy + 1] = ycy[dp_ypy + 1];
			ypy[dp_ypy + 2] = ycy[dp_ypy + 2];

			ypf[x] = ycf[x];
		}
		//memcpy(ypy, ycy, sizeof(float) * width_channel);
		//memcpy(ypf, ycf, sizeof(float) * width);
	}
#if 0
	floatData3* pTmp = (floatData3*)img_out_f;
	for (int i = 0; i < width_height; ++i)
	{
		out[i][0] = static_cast<unsigned char>(pTmp[i].a);
		out[i][1] = static_cast<unsigned char>(pTmp[i].b);
		out[i][2] = static_cast<unsigned char>(pTmp[i].c);
	}
#endif
}

// -----------------------------------------------------------------------------------------------------------------------------------------

inline void __Recursive_BF_X(cv::Vec3b* img, cv::Vec3b* out,
							float sigma_spatial, float sigma_range,
							int width, int height, int channel,
							float* buffer, const float* const range_table)
{
	const int width_height = width * height;
	const int width_channel = width * channel;
	const int width_height_channel = width * height * channel;

	float* img_out_f = buffer;
	float* img_temp = &img_out_f[width_height_channel];
	float* map_factor_a = &img_temp[width_height_channel];
	float* map_factor_b = &map_factor_a[width_height];
	float* slice_factor_a = &map_factor_b[width_height];
	float* slice_factor_b = &slice_factor_a[width_channel];
	float* line_factor_a = &slice_factor_b[width_channel];
	float* line_factor_b = &line_factor_a[width];

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

		if (y == height - 1) {
			for (int x = 0; x < width; x++)
			{
				float* in_factor = map_factor_a;
				int dp_tmp = x * channel;
				img_out_f[dp_tmp] = img_temp[dp_tmp];
				img_out_f[dp_tmp + 1] = img_temp[dp_tmp + 1];
				img_out_f[dp_tmp + 2] = img_temp[dp_tmp + 2];

				map_factor_b[x] = in_factor[x];
			}
		}

	}
}


inline void __Recursive_BF_Y(cv::Vec3b* img, cv::Vec3b* out,
							float sigma_spatial, float sigma_range,
							int width, int height, int channel,
							float* buffer, const float* const range_table)
{
	const int width_height = width * height;
	const int width_channel = width * channel;
	const int width_height_channel = width * height * channel;

	float* img_out_f = buffer;
	float* img_temp = &img_out_f[width_height_channel];
	float* map_factor_a = &img_temp[width_height_channel];
	float* map_factor_b = &map_factor_a[width_height];
	float* slice_factor_a = &map_factor_b[width_height];
	float* slice_factor_b = &slice_factor_a[width_channel];
	float* line_factor_a = &slice_factor_b[width_channel];
	float* line_factor_b = &line_factor_a[width];

	float alpha = static_cast<float>(exp(-sqrt(2.0) / (sigma_spatial * height)));
	float inv_alpha_ = 1 - alpha;
	float* ycy, * ypy, * xcy;
	cv::Vec3b* tcy, * tpy;

	float* in_factor = map_factor_a;
	float* ycf, * ypf, * xcf;

	int h1 = height - 1;
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
			cv::Vec3b tcy_pp = tcy[x];
			cv::Vec3b tpy_pp = tpy[x];
			unsigned char dr = abs((tcy_pp[0]) - (tpy_pp[0]));
			unsigned char dg = abs((tcy_pp[1]) - (tpy_pp[1]));
			unsigned char db = abs((tcy_pp[2]) - (tpy_pp[2]));
			/*
			++tcy;
			++tpy;
			*/
			int range_dist = (((dr << 1) + dg + db) >> 2);
			float weight = range_table[range_dist];
			float alpha_ = weight * alpha;
			for (int c = 0; c < channel; c++)
				*ycy++ = inv_alpha_ * (*xcy++) + alpha_ * (*ypy++);
			*ycf++ = inv_alpha_ * (*xcf++) + alpha_ * (*ypf++);

			if (y == h1) {
				ypf = line_factor_b;
				ypf[x] = in_factor[h1 * width + x];
				map_factor_b[h1 * width + x] = 0.5f * (map_factor_b[h1 * width + x] + ypf[x]);

				ycy = slice_factor_a;
				ypy = slice_factor_b;

				int tmp_dp = x * channel;
				ypy[tmp_dp] = img_temp[h1 * width_channel + tmp_dp];
				ypy[tmp_dp + 1] = img_temp[h1 * width_channel + tmp_dp + 1];
				ypy[tmp_dp + 2] = img_temp[h1 * width_channel + tmp_dp + 2];

				int k = 0;
				for (int x = 0; x < width; x++) {
					for (int c = 0; c < channel; c++) {
						int idx = (h1 * width + x) * channel + c;
						img_out_f[idx] = 0.5f * (img_out_f[idx] + ypy[k++]) / map_factor_b[h1 * width + x];
					}
				}
			}

		}
	}

}


inline void __Recursive_BF_Z(cv::Vec3b* img, cv::Vec3b* out,
	float sigma_spatial, float sigma_range,
	int width, int height, int channel,
	float* buffer, const float* const range_table)
{
	const int width_height = width * height;
	const int width_channel = width * channel;
	const int width_height_channel = width * height * channel;

	float* img_out_f = buffer;
	float* img_temp = &img_out_f[width_height_channel];
	float* map_factor_a = &img_temp[width_height_channel];
	float* map_factor_b = &map_factor_a[width_height];
	float* slice_factor_a = &map_factor_b[width_height];
	float* slice_factor_b = &slice_factor_a[width_channel];
	float* line_factor_a = &slice_factor_b[width_channel];
	float* line_factor_b = &line_factor_a[width];

	const int h1 = height - 1;

	float* ycf, * ypf, * xcf;
	float* ycy, * ypy, * xcy;

	ycf = line_factor_a;
	ypf = line_factor_b;

	ycy = slice_factor_a;
	ypy = slice_factor_b;

	float alpha = static_cast<float>(exp(-sqrt(2.0) / (sigma_spatial * height)));
	float inv_alpha_ = 1 - alpha;

	cv::Vec3b* tcy, * tpy;

	float* in_factor = map_factor_a;

	floatData3* pTmp = (floatData3*)img_out_f;
	for (/*int y = h1 - 1; y >= 0; y--*/int yt = 0; yt < h1 - 1; yt++)
	{
		int y = (h1 - 1) - yt;
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
			cv::Vec3b tcy_pp = tcy[x];
			cv::Vec3b tpy_pp = tpy[x];
			unsigned char dr = abs((tcy_pp[0]) - (tpy_pp[0]));
			unsigned char dg = abs((tcy_pp[1]) - (tpy_pp[1]));
			unsigned char db = abs((tcy_pp[2]) - (tpy_pp[2]));
			/*
			++tcy;
			++tpy;
			*/
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

			int dp = y * width + x;
			out[dp][0] = static_cast<unsigned char>(pTmp[dp].a);
			out[dp][1] = static_cast<unsigned char>(pTmp[dp].b);
			out[dp][2] = static_cast<unsigned char>(pTmp[dp].c);

			
			int dp_ypy = x * channel;
			ypy[dp_ypy] = ycy[dp_ypy];
			ypy[dp_ypy + 1] = ycy[dp_ypy + 1];
			ypy[dp_ypy + 2] = ycy[dp_ypy + 2];

			ypf[x] = ycf[x];
			
		}
	}

}

//-------------------------------------------------------------------------------------------------------------------


inline void __Recursive_BF_X_H(cv::Vec3b* img, cv::Vec3b* out,
	float sigma_spatial, float sigma_range,
	int width, int height, int channel,
	float* buffer, const float* const range_table, int iRowIndex)
{
	const int width_height = width * height;
	const int width_channel = width * channel;
	const int width_height_channel = width * height * channel;

	float* img_out_f = buffer;
	float* img_temp = &img_out_f[width_height_channel];
	float* map_factor_a = &img_temp[width_height_channel];
	float* map_factor_b = &map_factor_a[width_height];
	float* slice_factor_a = &map_factor_b[width_height];
	float* slice_factor_b = &slice_factor_a[width_channel];
	float* line_factor_a = &slice_factor_b[width_channel];
	float* line_factor_b = &line_factor_a[width];

	float alpha = static_cast<float>(exp(-sqrt(2.0) / (sigma_spatial * width)));
	float ypr, ypg, ypb, ycr, ycg, ycb;
	float fp, fc;
	float inv_alpha_ = 1 - alpha;

	//for (int y = 0; y < height; y++)
	//{
	int y = iRowIndex;
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

	if (y == height - 1) {
		for (int x = 0; x < width; x++)
		{
			float* in_factor = map_factor_a;
			int dp_tmp = x * channel;
			img_out_f[dp_tmp] = img_temp[dp_tmp];
			img_out_f[dp_tmp + 1] = img_temp[dp_tmp + 1];
			img_out_f[dp_tmp + 2] = img_temp[dp_tmp + 2];

			map_factor_b[x] = in_factor[x];
		}
	}

	//}
}

inline void __Recursive_BF_Y_V(cv::Vec3b* img, cv::Vec3b* out,
	float sigma_spatial, float sigma_range,
	int width, int height, int channel,
	float* buffer, const float* const range_table, int iColIndex)
{
	const int width_height = width * height;
	const int width_channel = width * channel;
	const int width_height_channel = width * height * channel;

	float* img_out_f = buffer;
	float* img_temp = &img_out_f[width_height_channel];
	float* map_factor_a = &img_temp[width_height_channel];
	float* map_factor_b = &map_factor_a[width_height];
	float* slice_factor_a = &map_factor_b[width_height];
	float* slice_factor_b = &slice_factor_a[width_channel];
	float* line_factor_a = &slice_factor_b[width_channel];
	float* line_factor_b = &line_factor_a[width];

	float alpha = static_cast<float>(exp(-sqrt(2.0) / (sigma_spatial * height)));
	float inv_alpha_ = 1 - alpha;
	float* ycy, * ypy, * xcy;
	cv::Vec3b* tcy, * tpy;

	float* in_factor = map_factor_a;
	float* ycf, * ypf, * xcf;

	int h1 = height - 1;

#if 1

	//for (int x = 0; x < width; x++)
	//{
	int x = iColIndex;
	for (int y = 1; y < height; y++)
	{
		int dp_1 = (y - 1) * width + x;
		int dp = (y) * width + x;

		tpy = &img[dp_1];
		tcy = &img[dp];

		xcy = &img_temp[y * width_channel + x * channel];
		ypy = &img_out_f[(y - 1) * width_channel + x * channel];
		ycy = &img_out_f[y * width_channel + x * channel];

		xcf = &in_factor[y * width + x];
		ypf = &map_factor_b[(y - 1) * width + x];
		ycf = &map_factor_b[y * width + x];

		cv::Vec3b tcy_pp = *tcy;
		cv::Vec3b tpy_pp = *tpy;
		unsigned char dr = abs((tcy_pp[0]) - (tpy_pp[0]));
		unsigned char dg = abs((tcy_pp[1]) - (tpy_pp[1]));
		unsigned char db = abs((tcy_pp[2]) - (tpy_pp[2]));

		int range_dist = (((dr << 1) + dg + db) >> 2);
		float weight = range_table[range_dist];
		float alpha_ = weight * alpha;

		for (int c = 0; c < channel; c++)
			ycy[c] = inv_alpha_ * (xcy[c]) + alpha_ * (ypy[c]);
		*ycf = inv_alpha_ * (*xcf) + alpha_ * (*ypf);

		if (y == h1) {
			ypf = line_factor_b;
			ypf[x] = in_factor[h1 * width + x];
			map_factor_b[h1 * width + x] = 0.5f * (map_factor_b[h1 * width + x] + ypf[x]);

			ycy = slice_factor_a;
			ypy = slice_factor_b;

			int tmp_dp = x * channel;
			ypy[tmp_dp] = img_temp[h1 * width_channel + tmp_dp];
			ypy[tmp_dp + 1] = img_temp[h1 * width_channel + tmp_dp + 1];
			ypy[tmp_dp + 2] = img_temp[h1 * width_channel + tmp_dp + 2];

			int k = 0;
			for (int x = 0; x < width; x++) {
				for (int c = 0; c < channel; c++) {
					int idx = (h1 * width + x) * channel + c;
					img_out_f[idx] = 0.5f * (img_out_f[idx] + ypy[k++]) / map_factor_b[h1 * width + x];
				}
			}
		}

	}
	//}

#else

#endif
}

inline void __Recursive_BF_Z_V(cv::Vec3b* img, cv::Vec3b* out,
	float sigma_spatial, float sigma_range,
	int width, int height, int channel,
	float* buffer, const float* const range_table, int iColIndex)
{
	//if (iRowIndex < 0 || iRowIndex >= height - 2) return;
	const int width_height = width * height;
	const int width_channel = width * channel;
	const int width_height_channel = width * height * channel;

	float* img_out_f = buffer;
	float* img_temp = &img_out_f[width_height_channel];
	float* map_factor_a = &img_temp[width_height_channel];
	float* map_factor_b = &map_factor_a[width_height];
	float* slice_factor_a = &map_factor_b[width_height];
	float* slice_factor_b = &slice_factor_a[width_channel];
	float* line_factor_a = &slice_factor_b[width_channel];
	float* line_factor_b = &line_factor_a[width];

	const int h1 = height - 1;

	float* ycf, * ypf, * xcf;
	float* ycy, * ypy, * xcy;

	ycf = line_factor_a;
	ypf = line_factor_b;

	ycy = slice_factor_a;
	ypy = slice_factor_b;

	float alpha = static_cast<float>(exp(-sqrt(2.0) / (sigma_spatial * height)));
	float inv_alpha_ = 1 - alpha;

	cv::Vec3b* tcy, * tpy;

	float* in_factor = map_factor_a;

	floatData3* pTmp = (floatData3*)img_out_f;
	//int yt = iRowIndex;
	int x = iColIndex;
	//for (int x = 0; x < width; x++)
	//{
		for (int yt = 0; yt < h1 - 1; yt++)
		{
			int y = (h1 - 1) - yt;

			int dp_bottom = (y + 1) * width + x;
			int dp_curr = (y) * width + x;
			tpy = &img[dp_bottom];
			tcy = &img[dp_curr];
			xcy = &img_temp[y * width_channel + x * channel];
			floatData3* ycy_ = (floatData3*)ycy; //slice_factor_a
			ycy_ += x;
			floatData3* ypy_ = (floatData3*)ypy; //slice_factor_b
			ypy_ += x;

			floatData3* out_ = (floatData3*)img_out_f;// [y * width_channel + x * channel] ;
			out_ += dp_curr;

			xcf = &in_factor[dp_curr];
			float* ycf_ = &ycf[x]; //line_factor_a
			float* ypf_ = &ypf[x]; //line_factor_b
			float* factor_ = &map_factor_b[dp_curr];

			cv::Vec3b tcy_pp = *tcy;
			cv::Vec3b tpy_pp = *tpy;
			unsigned char dr = abs((tcy_pp[0]) - (tpy_pp[0]));
			unsigned char dg = abs((tcy_pp[1]) - (tpy_pp[1]));
			unsigned char db = abs((tcy_pp[2]) - (tpy_pp[2]));

			int range_dist = (((dr << 1) + dg + db) >> 2);
			float weight = range_table[range_dist];
			float alpha_ = weight * alpha;

			float fcc = inv_alpha_ * (*xcf) + alpha_ * (*ypf_);
			*ycf_ = fcc;
			*factor_ = 0.5f * (*factor_ + fcc);

			for (int c = 0; c < channel; c++)
			{
				float ycc = inv_alpha_ * (xcy[c]) + alpha_ * ((*ypy_)[c]);
				(*ycy_)[c] = ycc;
				(*out_)[c] = 0.5f * ((*out_)[c] + ycc) / (*factor_);
			}

			int dp = y * width + x;
			out[dp][0] = static_cast<unsigned char>(pTmp[dp].a);
			out[dp][1] = static_cast<unsigned char>(pTmp[dp].b);
			out[dp][2] = static_cast<unsigned char>(pTmp[dp].c);

			
			int dp_ypy = x * channel;
			ypy[dp_ypy] = ycy[dp_ypy];
			ypy[dp_ypy + 1] = ycy[dp_ypy + 1];
			ypy[dp_ypy + 2] = ycy[dp_ypy + 2];

			ypf[x] = ycf[x];
			
		}
	//}
}
//---------------------------------------------------------------------------------------------------------------------

typedef std::function<void(cv::Vec3b * img, cv::Vec3b * out,
	float sigma_spatial, float sigma_range,
	int width, int height, int channel,
	float* buffer, const float* const range_table, int iColIndex)> kFunc;

inline void __Recursive_BF_Main(cv::Vec3b* img, cv::Vec3b* out,
	float sigma_spatial, float sigma_range,
	int width, int height, int channel,
	float* buffer, const float* const range_table)
{
	
	auto f1 = __Recursive_BF_X_H;
	auto f2 = __Recursive_BF_Y_V;
	auto f3 = __Recursive_BF_Z_V;

	int b1 = 0;
	int e1 = width / 2;
	int b2 = e1;
	int e2 = width;

	int b_1 = 0;
	int e_1 = height / 2;
	int b_2 = e1;
	int e_2 = height;

	auto f = [&](int begin, int end, kFunc func) {
		for (int i = begin; i < end; i++)
		{
			func(img, out, sigma_spatial, sigma_range, width, height, channel, buffer, range_table, i);
		}
	};

	auto f_inv = [&](int begin, int end, kFunc func) {
		for (int i = end; i >= begin; i--)
		{
			func(img, out, sigma_spatial, sigma_range, width, height, channel, buffer, range_table, i);
		}
	};

	std::thread t1{ f, b_1, e_1, f1 };
	std::thread t2{ f, b_2, e_2, f1 };

	t1.join();
	t2.join();

	t1 = std::thread{f, b1, e1, f2};
	t2 = std::thread{f, b2, e2, f2};

	t1.join();
	t2.join();

	t1 = std::thread{ f, b1, e1, f3 };
	t2 = std::thread{ f, b2, e2, f3 };

	t1.join();
	t2.join();

	//__Recursive_BF_X(img, out, sigma_spatial, sigma_range, width, height, channel, buffer, range_table);
	//__Recursive_BF_Y_V(img, out, sigma_spatial, sigma_range, width, height, channel, buffer, range_table);
	//__Recursive_BF_Z(img, out, sigma_spatial, sigma_range, width, height, channel, buffer, range_table);
}

// -----------------------------------------------------------------------------------------------------------------------------------------

int main() {

	std::cout << "Hello BF" << std::endl;

	cv::Mat src = cv::imread("test1.jpg");
	cv::Mat out = src.clone();


	const float sigma_range = 0.1;
	const float sigma_spatial = 0.02;

	const int width = src.cols;
	const int height = src.rows;
	const int channel = 3;

	const int width_height = width * height;
	const int width_channel = width * channel;
	const int width_height_channel = width * height * channel;

	float* buffer = buffer = new float[(width_height_channel + width_height
		+ width_channel + width) * 2];

	float range_table[QX_DEF_CHAR_MAX + 1];
	float inv_sigma_range = 1.0f / (sigma_range * QX_DEF_CHAR_MAX);
	for (int i = 0; i <= QX_DEF_CHAR_MAX; i++)
		range_table[i] = static_cast<float>(exp(-i * inv_sigma_range));

	__Recursive_BF_Main((cv::Vec3b*)src.data, (cv::Vec3b*)out.data, sigma_spatial, sigma_range, width, height, channel, buffer, range_table);

	delete[] buffer;

	cv::namedWindow("image", cv::WINDOW_NORMAL);
	cv::imshow("image", out);
	cv::namedWindow("image_in", cv::WINDOW_NORMAL);
	cv::imshow("image_in", src);
	cv::waitKey(0);

	return 0;
}
#endif