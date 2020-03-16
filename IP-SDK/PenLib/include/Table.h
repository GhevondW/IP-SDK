#ifndef _TABLE_H__
#define _TABLE_H__

#include "Row.h"

namespace pen
{

	typedef Row<std::string> AnyValRow;
	typedef Row<std::string> ColumnRow;
	typedef std::vector<AnyValRow*> TableDS; // DS AS Data Structure

	class Table 
	{
	public:

		Table();
		Table(const std::initializer_list<std::string>& cols);
		Table(const ColumnRow& cols);

		Table(const Table& other);
		Table(Table&& other);

		~Table()
		{
			if (!_bIsWrapped) {
				for (int i = 0; i < _Data.size(); i++)
				{
					delete _Data[i];
					_Data[i] = nullptr;
				}
			}
		}

	public:

		Table& operator=(const Table& other);
		Table& operator=(Table&& other);

	public:



	private:
		ColumnRow _Cols;
		TableDS _Data;

		bool _bIsWrapped{false};
	};
};

#endif // !_TABLE_H__

