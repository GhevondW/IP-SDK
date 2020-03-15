#include "Row.h"

using namespace pen;

Row::Row()
{

}

Row::Row(std::initializer_list<std::any> list)
{
	size_t size = list.size();
	if (size > 0) {
		_Values.insert(_Values.end(), list.begin(), list.end());
	}
}

Row::Row(const std::vector<Entity>& values)
	:_Values(values)
{
}

Row::Row(const Row& other)
	:_Values(other._Values)
{

}

Row::Row(Row&& other)
	:_Values(std::move(other._Values))
{

}

Row::~Row()
{

}

Row& Row::operator=(const Row& other)
{
	_Values = other._Values;
	return *this;
}

Row& Row::operator=(Row&& other)
{
	_Values = std::move(other._Values);
	return *this;
}


int Row::GetLength() const
{
	return _Values.size();
}


const Row::Entity& Row::EntityAt(const size_t x)
{
	int size = GetLength();
	if (x >= 0 && x < size) {
		return _Values[x];
	}
	throw "Invalid Operation";
}

template<class T>
const T& Row::ValueAt(const size_t x)
{
	const Row::Entity& ent = EntityAt(x);
	if (ent.has_value()) {
		if (ent.type().name() == typeid(T).name()) {
			return std::any_cast<T>(ent);
		}
	}
	throw "Invalid Operation";
}

void Row::SetAt(const size_t x, const std::any& value)
{
	//TODO
}