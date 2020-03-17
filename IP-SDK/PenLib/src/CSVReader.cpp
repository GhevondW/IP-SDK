#include "CSVReader.h"

using namespace pen;

DirManager CSVReader::G_DirManager = {};

Table* CSVReader::ReadCSV(const std::string& filePath)
{
	return {};
}

std::vector<std::string> CSVReader::GetData(const std::string& fileName)
{
	std::ifstream file(fileName);

	std::vector<std::string> dataList;

	std::string line = "";
	// Iterate through each line and split the content using delimeter
	while (getline(file, line))
	{
		//boost::algorithm::split(vec, line, boost::is_any_of(delimeter));
		dataList.push_back(line);
	}
	// Close the File
	file.close();

	return dataList;
}