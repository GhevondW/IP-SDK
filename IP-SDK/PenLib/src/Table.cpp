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
				_Cols.PushBack("col_" + std::to_string(i));
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