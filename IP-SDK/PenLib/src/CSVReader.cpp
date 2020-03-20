#include "CSVReader.h"
#include <sstream>

using namespace pen;

DirManager CSVReader::G_DirManager = {};

Table* CSVReader::ReadCSV(const std::string& filePath)
{
	return {};
}

std::vector<std::vector<std::string>> CSVReader::GetData(const std::string& fileName)
{
	std::ifstream file(fileName);

	std::vector<std::vector<std::string>> dataList;

	std::string line = "";
	// Iterate through each line and split the content using delimeter
	while (getline(file, line))
	{
		//boost::algorithm::split(vec, line, boost::is_any_of(delimeter));
		std::vector<std::string> elems;
		std::stringstream ss(line);
		std::string value;
		const char delim = ',';
		while (std::getline(ss, value, delim)) {
			elems.push_back(value);
		}
		dataList.push_back(elems);
	}
	// Close the File
	file.close();

	return dataList;
}