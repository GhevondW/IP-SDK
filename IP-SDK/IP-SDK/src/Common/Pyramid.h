#ifndef _PYRAMID_H_
#define _PYRAMID_H_

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
#include <functional>
#include <memory>

struct Level
{
	size_t	_index{};
	cv::Mat _image{};
	cv::Mat _grad{};
	cv::Mat _bilateral{};
	cv::Mat _shine{};
	cv::Mat _healing{};
	cv::Mat _tmp{};
	cv::Mat _tmp2{};
};

#define N 7
static std::array<Level, N> pyramid;

struct IImageGetter
{
	virtual cv::Mat& GetImageRef(size_t) = 0;
	virtual cv::Mat GetImageCopy(size_t) = 0;
};

struct ImageGetter : public IImageGetter
{
	cv::Mat& GetImageRef(size_t i) override { return pyramid[i]._image; };
	cv::Mat GetImageCopy(size_t i) override { return pyramid[i]._image.clone(); };
};

struct GradGetter : public IImageGetter
{
	cv::Mat& GetImageRef(size_t i) override { return pyramid[i]._grad; };
	cv::Mat GetImageCopy(size_t i) override { return pyramid[i]._grad.clone(); };
};

struct BilateralGetter : public IImageGetter
{
	cv::Mat& GetImageRef(size_t i) override { return pyramid[i]._bilateral; };
	cv::Mat GetImageCopy(size_t i) override { return pyramid[i]._bilateral.clone(); };
};

struct HealingGetter : public IImageGetter
{
	cv::Mat& GetImageRef(size_t i) override { return pyramid[i]._healing; };
	cv::Mat GetImageCopy(size_t i) override { return pyramid[i]._healing.clone(); };
};

struct ShineGetter : public IImageGetter
{
	cv::Mat& GetImageRef(size_t i) override { return pyramid[i]._shine; };
	cv::Mat GetImageCopy(size_t i) override { return pyramid[i]._shine.clone(); };
};

struct TempGetter : public IImageGetter
{
	cv::Mat& GetImageRef(size_t i) override { return pyramid[i]._tmp; };
	cv::Mat GetImageCopy(size_t i) override { return pyramid[i]._tmp.clone(); };
};

struct Temp2Getter : public IImageGetter
{
	cv::Mat& GetImageRef(size_t i) override { return pyramid[i]._tmp2; };
	cv::Mat GetImageCopy(size_t i) override { return pyramid[i]._tmp2.clone(); };
};


Level& GetLevel(size_t index)
{
	return pyramid[index];
}

cv::Mat& GetLevelImage(size_t index)
{
	return pyramid[index]._image;
}

void CreatePyramid(cv::Mat& src)
{
	GetLevel(0)._image = src.clone();
	for (size_t i = 1; i < N; i++)
	{
		int h = GetLevel(i - 1)._image.rows;
		int w = GetLevel(i - 1)._image.cols;
		cv::resize(GetLevelImage(i - 1), GetLevelImage(i), cv::Size(w/2,h/2));
	}
}

void Iterate(std::function<void(cv::Mat& image, size_t index)> func, std::unique_ptr<IImageGetter> getter)
{
	for (size_t i = 0; i < N; i++)
	{
		func(getter->GetImageRef(i), i);
	}
}

#endif // !_PYRAMID_H_

