#if 0
#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <string.h>
#include <thread>
#include <DirManager.h>

double medianMat(cv::Mat Input, int nVals) {
	// COMPUTE HISTOGRAM OF SINGLE CHANNEL MATRIX
	float range[] = { 0, nVals };
	const float* histRange = { range };
	bool uniform = true; bool accumulate = false;
	cv::Mat hist;
	calcHist(&Input, 1, 0, cv::Mat(), hist, 1, &nVals, &histRange, uniform, accumulate);

	// COMPUTE CUMULATIVE DISTRIBUTION FUNCTION (CDF)
	cv::Mat cdf;
	hist.copyTo(cdf);
	for (int i = 1; i <= nVals - 1; i++) {
		cdf.at<float>(i) += cdf.at<float>(i - 1);
	}
	cdf /= Input.total();

	// COMPUTE MEDIAN
	double medianVal;
	for (int i = 0; i <= nVals - 1; i++) {
		if (cdf.at<float>(i) >= 0.5) { medianVal = i;  break; }
	}
	return medianVal;
}

int main() {

	std::cout << "Edge detection" << std::endl;

	pen::DirManager dir("obj_test_res", "tiff");

	auto& images = dir.GetFilesWithExt();
#if 1
	for (int i = 0; i < images.size(); i++)
	{
		cv::Mat src = cv::imread(images[i], cv::IMREAD_GRAYSCALE);
		cv::GaussianBlur(src, src, cv::Size(5, 5), 0.2, 0.2);
		double sigma = 0.33;
		int median = medianMat(src, 0xff);
		int lower = int(std::max(0., (1.0 - sigma) * median));
		int	upper = int(std::min(255., (1.0 + sigma) * median));
		//cv::GaussianBlur(src, src, cv::Size(5, 5), 2, 2);
		cv::Mat canny;
		//cv::threshold(src, canny, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);
		Canny(src, canny, lower, upper, 3);

		cv::imwrite("obj_edges\\file_" + std::to_string(i) + ".tiff", canny);
	}
#else
	cv::Mat src = cv::imread(images[1658], cv::IMREAD_GRAYSCALE);
	double sigma = 0.33;
	int median = medianMat(src, 0xff);
	int lower = int(std::max(0., (1.0 - sigma) * median));
	int	upper = int(std::min(255., (1.0 + sigma) * median));
	//cv::GaussianBlur(src, src, cv::Size(5, 5), 2, 2);
	//cv::equalizeHist(src, src);
	cv::Mat canny;
	Canny(src, canny, lower, upper, 3);
	//cv::threshold(src, canny, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);

	cv::namedWindow("image_in", cv::WINDOW_NORMAL);
	cv::imshow("image_in", canny);
	cv::waitKey(0);
#endif
	return 0;
}
#endif