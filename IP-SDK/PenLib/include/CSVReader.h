#ifndef _CSV_READER_H_
#define _CSV_READER_H_

#include "Table.h"
#include "DirManager.h"

namespace pen
{
	class CSVReader
	{
	public:
		static Table ReadCSV(const std::string& filePath);
	private:
		static DirManager G_DirManager;
	};

};

#endif // !_CSV_READER_H_

