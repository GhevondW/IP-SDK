#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <string.h>
#include <thread>
#include <DirManager.h>
#include <Table.h>
#include <CSVReader.h>

int main() {

	
	std::cout << "Hello World" << std::endl;

	pen::Table* table = pen::CSVReader::ReadCSV("C:\\Users\\Ghevond\\Desktop\\Dataset\\DFBS_Unique.csv");

	int dx = table->GetValue<int>(1, "dx");

	delete table;
	return 0;
}