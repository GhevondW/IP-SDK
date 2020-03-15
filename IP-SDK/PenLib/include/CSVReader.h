#ifndef _CSV_READER_H_
#define _CSV_READER_H_

#include <memory>
#include "Table.h"
#include "DirManager.h"

namespace pen
{
	class CSVReader
	{
	public:
		static std::shared_ptr<Table> ReadCSV(const std::string& filePath);
	private:
		static DirManager G_DirManager;
	};
};

#endif // !_CSV_READER_H_

