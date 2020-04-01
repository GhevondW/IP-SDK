#include "Table.h"

using namespace pen;

/*
typedef Row<std::string> AnyValRow;
typedef Row<std::string> ColumnRow;
typedef std::vector<AnyValRow*> TableDS; // DS AS Data Structure
*/

Table::Table()
	:_Cols(),
	_Data(),
	_bIsWrapped(false)
{

}

Table::Table(std::initializer_list<std::string>&& cols)
	:_Cols(cols),
	_Data(),
	_bIsWrapped(false)
{
}

Table::Table(const ColumnRow& cols)
	:_Cols(cols),
	_Data(),
	_bIsWrapped(false)
{

}

Table::Table(MoveTableDS data)
	:_bIsWrapped(false)
{
	bool check_data{true};
	if (data.size() > 0) {
		auto begin = data.begin();
		int begin_size = begin->GetLength();
		while (begin != data.end())
		{
			int current_size = begin->GetLength();
			if (current_size != begin_size) {
				check_data = false;
				break;
			}
			++begin;
		}

		if (check_data) {
			for (int i = 0; i < begin_size; i++)
			{
				_Cols.PushBack(DEF_COL_NAME + std::to_string(i));
			}
			int size = data.size();
			for (int i = 0; i < size; i++)
			{
				_Data.push_back(new AnyValRow(std::move(data[i])));
			}
		}
		else {
			InitDefault();
		}
		
	}
	else {
		InitDefault();
	}
}

Table::Table(CopyTableDS data)
{
	InitDefault();
	if (CheckInputData(data)) {
		InitTable(data);
	}
}

Table::Table(const Table& other) //This construction will wrap the Table
	:_Cols(other._Cols),
	_bIsWrapped(true)
{
	auto begin_other = other._Data.begin();
	auto end_other = other._Data.end();
	_Data.insert(_Data.end(), begin_other, end_other);
}

Table::Table(Table&& other)
	:_bIsWrapped(false)
{
	auto begin_other = other._Data.begin();
	auto end_other = other._Data.end();
	_Data.insert(_Data.end(), begin_other, end_other);
	other._Data.clear();	
}

Table::~Table()
{
	if (!_bIsWrapped) {
		for (int i = 0; i < GetDataCount(); i++)
		{
			delete _Data[i];
			_Data[i] = nullptr;
		}
		_Data.clear();
	}
}


Table& Table::operator=(const Table& other) //This operator will Wrap the Table
{
	_Cols = other._Cols;

	_bIsWrapped = true;
	auto begin_other = other._Data.begin();
	auto end_other = other._Data.end();
	_Data.insert(_Data.end(), begin_other, end_other);

	return *this;
}

Table& Table::operator=(Table&& other)
{
	_Cols = other._Cols;

	_bIsWrapped = true;
	auto begin_other = other._Data.begin();
	auto end_other = other._Data.end();
	_Data.insert(_Data.end(), begin_other, end_other);
	other._Data.clear();

	return *this;
}


std::shared_ptr<Table> Table::DeepCopy()
{
	//TODO
	/*return std::make_shared<Table>(new Table());*/
	return nullptr;
}

int Table::GetColumnsCount() const
{
	return _Cols.GetLength();
}

int Table::GetDataCount() const
{
	return _Data.size();
}


void Table::InitDefault()
{
	_Data = {};
	_Cols = {};
}

bool Table::CheckInputData(CopyTableDS data)
{
	if (data.empty()) return false;
	auto begin = data.begin();
	int size = begin->size();

	if (!((*begin).at(0).empty())) return false;

	while (begin != data.end())
	{
		if (begin->size() != size) {
			return false;
		}
		++begin;
	}
	return true;
}

void Table::InitTable(CopyTableDS data)
{
	_Cols = *data.begin();
	_Cols[0] = DEF_INDEX_NAME;
	auto start = data.begin() + 1;
	while (start != data.end())
	{
		_Data.push_back(new AnyValRow(*start, &_Cols));
		++start;
	}
}


AnyValRow* Table::GetRow(const int index)
{
	int count = GetDataCount();
	if (index >= 0 && index < count) {
		return _Data[index];
	}
	return nullptr;
}

AnyValRow* Table::GetFirstRowIf(CheckFunction fCheck)
{
	auto begin = _Data.begin();
	while (begin != _Data.end())
	{
		if (fCheck(**begin)) {
			return *begin;
		}
		++begin;
	}
	return nullptr;
}



const std::string Table::DEF_INDEX_NAME = "index";
const std::string Table::DEF_COL_NAME = "col_";