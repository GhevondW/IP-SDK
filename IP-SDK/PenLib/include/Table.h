#ifndef _TABLE_H__
#define _TABLE_H__

#include <functional>
#include "Row.h"

namespace pen
{
	
	typedef Row<std::string> AnyValRow;
	typedef Row<std::string> ColumnRow;
	typedef std::vector<AnyValRow*> TableDS; // DS AS Data Structure
	typedef std::vector<AnyValRow>&& MoveTableDS;
	typedef const std::vector<std::vector<std::string>>& CopyTableDS;

	class StringCast
	{
		friend class Table;
		StringCast(const std::string& val) 
			:_val(val)
		{}

		operator int() {
			if (IsNumber(_val)) {
				return std::stoi(_val);
			};
			throw "Cast Exception";
		}

		operator float() {
			if (IsNumber(_val)) {
				return std::stof(_val);
			};
			throw "Cast Exception";
		}

		operator double() {
			if (IsNumber(_val)) {
				return std::stod(_val);
			};
			throw "Cast Exception";
		}

		bool IsNumber(const std::string& s)
		{
			if (s.size() == 0) return false;
			return((strspn(s.c_str(), "-.0123456789") == s.size()) && s[0] != '.');
		}

	private:
		const std::string& _val;
	};

	class Table 
	{
		typedef std::function<bool( AnyValRow & row)> CheckFunction;
	public:

		Table();
		Table(std::initializer_list<std::string>&& cols);
		Table(const ColumnRow& cols);
		Table(MoveTableDS data);
		Table(CopyTableDS data);

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

		template<class T>
		auto GetValue(const int index, const std::string& colName)
		{
			int size = GetDataCount();
			if (index >= 0 && index < size) {
				const auto& row = _Data[index];
				const std::string& value = row->LocCol(colName);
				return (T)StringCast(value);
			}
		}

		AnyValRow* GetRow(const int index);
		AnyValRow* GetFirstRowIf(CheckFunction fCheck);

	private:

		void InitDefault();
		bool CheckInputData(CopyTableDS data);
		void InitTable(CopyTableDS data);

	private:
		ColumnRow _Cols;
		TableDS _Data;

		bool _bIsWrapped{false};

	private:

		static const std::string DEF_INDEX_NAME;
		static const std::string DEF_COL_NAME;

	};
};

#endif // !_TABLE_H__

