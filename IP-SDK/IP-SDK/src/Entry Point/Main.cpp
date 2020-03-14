#if 0
#include "GlobalDefines.h"
#include "Image.h"
#include <opencv2/opencv.hpp>
#include <CSVReader.h>
#include <thread>
#include "Rect.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace sdk;

#if 0

void ProcessImage(sdk::Image* src)
{
	int iWidth = src->GetWidth();
	int iHeight = src->GetHeight();

	for (int y = 0; y < iHeight; y++)
	{
		for (int x = 0; x < iWidth; x++)
		{
			src->at<Pixel24>(x, y)[0] = 0;
			src->at<Pixel24>(x, y)[1] = 0;
			src->at<Pixel24>(x, y)[2] = 255;
		}
	}

}

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
	
	/*
	cv::Mat img = cv::imread("test.jpg");
	int iWidth = img.cols;
	int iHeight = img.rows;
	int iChannelsCount = img.channels();
	void* pData = (void*)img.data;
	sdk::Image* sdkImg = sdk::Image::Make()
		.SetBpp(1)
		.SetComponenetCount(iChannelsCount)
		.SetWidth(iWidth)
		.SetHeight(iHeight)
		.SetSrcData(pData)
		.Wrap()
		.Build();

	ProcessImage(sdkImg);

	cv::namedWindow("image", cv::WINDOW_NORMAL);
	cv::imshow("image", img);
	cv::waitKey(0);
	*/

	pen::DirManager manager("C:\\Users\\Ghevond\\Desktop\\Utils\\SQL8.txt");

	return 0;
}

#endif

#define MAXUSHORT 0xffff

struct Region
{
	int begin{};
	int end{};
};

void Invert(cv::Mat& image) {
	int iWidth = image.cols;
	int iHeight = image.rows;

	unsigned int count = std::thread::hardware_concurrency();
	--count;

	const int TileHeight = iHeight / count;
	std::vector<Region> regions;
	for (int i = 0; i < count - 1; i++)
	{
		regions.push_back({TileHeight * i, TileHeight * (i + 1)});
	}
	regions.push_back({regions.rbegin()->end, iHeight});

	auto processor = [&](const Region& reg) {
	
		int begin = reg.begin;
		int end = reg.end;

		for (int y = begin; y < end; y++)
		{
			for (int x = 0; x < iWidth; x++)
			{
				image.at<ushort>(y, x) = MAXUSHORT - image.at<ushort>(y, x);
			}
		}

	};

	std::vector<std::thread> threads;
	for (int i = 0; i < count; i++)
	{
		std::thread thread(processor, regions[i]);
		threads.push_back(std::move(thread));
	}

	for (int i = 0; i < threads.size(); i++)
	{
		if (threads[i].joinable())
		{
			threads[i].join();
		}
	}

}

void CreateCropImage(cv::Mat& src, cv::Mat& dst, const std::pair<int , int>& coord)
{
	int iHeight = src.rows;
	int x = coord.first;
	int y = iHeight - coord.second;
	const int size_x = 50;
	const int size_y = 100;
	cv::Rect rect{x - size_x, y - size_x, size_x << 1, size_y << 1};

	cv::Mat croppedRef(src, rect);
	//src.at<ushort>(y, x) = 0;
	croppedRef.copyTo(dst);

}

int main() {

#if 0
	const std::vector<std::pair<int, int>> coords
	{
		{4929, 4894},
		{4916, 4597},
		{4904, 4667},
		{4901, 4702},
		{4901, 4904},
		{4887, 4619},
		{4873, 4589},
		{4855, 4634},
		{4853, 4871},
		{4846, 4779},
		{4815, 4877},
		{4805, 4706},
		{4798, 4795},
		{4773, 4624},
		{4757, 4589},
		{4742, 4648},
		{4732, 4868},
		{4686, 4642},
		{4686, 4721},
		{4665, 4790},
		{4637, 4599},
		{4628, 4906},
		{4618, 4697},
		{4618, 4689},
	};

	cv::Mat img = cv::imread("fbs1477_cor.tiff", cv::IMREAD_ANYDEPTH);
	cv::Mat gray;



	Invert(img);

	std::for_each(coords.begin(), coords.end(), [&](const std::pair<int, int> coord) {
		cv::Mat result_crop;
		const std::string name = "obj_images//fbs1477_" + std::to_string(coord.first) + "_" + std::to_string(coord.second) + ".tiff";
		CreateCropImage(img, result_crop, coord);
		cv::imwrite(name, result_crop);
	});

	

	cv::namedWindow("image", cv::WINDOW_NORMAL);
	cv::imshow("image", img);
	cv::waitKey(0);
#endif

	std::vector<std::vector<cv::Point>> thresh_callback(cv::Mat & gray, cv::Mat & canny);
	sdk::Rect GetBoundingRect(const std::vector<std::vector<cv::Point>> & contours);
	void DrawRect(cv::Mat & img, sdk::Rect rect);

	pen::DirManager paths("obj_images");

	const std::vector<std::string>& all_paths = paths.GetFilesPath();

	for (int i = 0; i < all_paths.size() - 5; i++)
	{
		cv::Mat image = cv::imread(all_paths[i], cv::IMREAD_GRAYSCALE);
		cv::Mat canny;
		cv::equalizeHist(image, image);
#if 1
		auto result = thresh_callback(image, canny);

		sdk::Rect cent_rect = GetBoundingRect(result);

		DrawRect(image, cent_rect);
#endif
		//cv::imwrite("obj_res\\file_"+std::to_string(i) + ".png", image);
		cv::namedWindow("image" + std::to_string(i), cv::WINDOW_NORMAL);
		cv::imshow("image" + std::to_string(i), canny);
	}
#if 0
	cv::Mat image = cv::imread(all_paths[0], cv::IMREAD_GRAYSCALE);
	cv::Mat canny;

	auto result = thresh_callback(image, canny);

	sdk::Rect cent_rect = GetBoundingRect(result);

	DrawRect(image, cent_rect);

	cv::namedWindow("image", cv::WINDOW_NORMAL);
	cv::imshow("image", image);
	cv::waitKey(0);
#endif
	cv::waitKey(0);
	std::cout << "Hello world" << std::endl;
	return 0;
}

using namespace std;
using namespace cv;
sdk::Rect GetBoundingRect(const std::vector<std::vector<cv::Point>>& contours)
{
	const int obj_x = 50;
	const int obj_y = 50;

	sdk::Point point{obj_x, obj_y};

	for (int i = 1; i < contours.size(); i++)
	{
		const std::vector<cv::Point>& cnt = contours[i];

		int max_x = cnt[0].x;
		int min_x = cnt[0].x;
		int max_y = cnt[0].y;
		int min_y = cnt[0].y;

		for (int j = 1; j < cnt.size(); j++)
		{
			auto p = cnt[j];
			int x = p.x;
			int y = p.y;

			if (x > max_x) {
				max_x = x;
			}
			else if (x < min_x) {
				min_x = x;
			}

			if (y > max_y) {
				max_y = y;
			}
			else if (y < min_y) {
				min_y = y;
			}
		}

		sdk::Rect cnt_rect{min_y, max_y, min_x, max_x};
		if (cnt_rect.Contains(point)) {
			cnt_rect.SetTop(cnt_rect.GetTop() - 2);
			cnt_rect.SetBottom(cnt_rect.GetBottom() + 2);
			cnt_rect.SetLeft(cnt_rect.GetLeft() - 2);
			cnt_rect.SetRight(cnt_rect.GetRight() + 2);
			return cnt_rect;
		}

	}
	return{};
}

void DrawRect(cv::Mat& img, sdk::Rect rect)
{
	for (int y = rect.GetTop(); y <= rect.GetBottom(); y++)
	{
		for (int x = rect.GetLeft(); x <= rect.GetRight(); x++)
		{
			if (x == rect.GetLeft() || x == rect.GetRight() || y == rect.GetTop() || y == rect.GetBottom()) {
				img.at<uchar>(y, x) = 255;
			}
		}
	}
}

std::vector<std::vector<cv::Point>> thresh_callback(cv::Mat& gray, cv::Mat& canny)
{

	vector<vector<cv::Point>> contours;
	vector<Vec4i> hierarchy;

	cv::GaussianBlur(gray, gray, cv::Size(25, 25), 2, 2);

	/// Detect edges using canny
	Canny(gray, canny, 150, 200, 3, false);
	//cv::threshold(gray, canny, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);
	//adaptiveThreshold(gray, canny, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 51, 12);
	/// Find contours
	//findContours(canny, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	/// Draw contours

#if 0
	RNG rng(12345);
	Mat drawing = Mat::zeros(canny.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point());
	}

	/// Show in a window
	namedWindow("Contours", WINDOW_NORMAL);
	imshow("Contours", drawing);
#endif
	return contours;
}
#endif