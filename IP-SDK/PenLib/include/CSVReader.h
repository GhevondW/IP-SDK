#ifndef _CSV_READER_H_
#define _CSV_READER_H_

#include <memory>
#include <fstream>
#include "Table.h"
#include "DirManager.h"

namespace pen
{
	class CSVReader
	{
	public:
		static Table* ReadCSV(const std::string& filePath);

	public:

		static std::vector<std::vector<std::string>> GetData(const std::string& fileName);

	private:
		static DirManager G_DirManager;
	};
};

#endif // !_CSV_READER_H_

