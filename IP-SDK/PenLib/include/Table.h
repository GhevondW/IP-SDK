#ifndef _TABLE_H__
#define _TABLE_H__

#include "Row.h"

namespace pen
{

	typedef Row<std::string> AnyValRow;
	typedef Row<std::string> ColumnRow;
	typedef std::vector<AnyValRow*> TableDS; // DS AS Data Structure
	typedef std::vector<AnyValRow>&& MoveTableDS;

	class Table 
	{
	public:

		Table();
		Table(std::initializer_list<std::string>&& cols);
		Table(const ColumnRow& cols);
		Table(MoveTableDS data);

		Table(const Table& other); //This construction will wrap the Table
		Table(Table&& other);

		~Table();

	public:

		Table& operator=(const Table& other); //This operator will Wrap the Table
		Table& operator=(Table&& other);

	public:

		std::shared_ptr<Table> DeepCopy();
		inline int GetColumnsCount() const;
		inline int GetDataCount() const;

	private:

		void InitDefault();

	private:
		ColumnRow _Cols;
		TableDS _Data;

		bool _bIsWrapped{false};
	};
};

#endif // !_TABLE_H__

