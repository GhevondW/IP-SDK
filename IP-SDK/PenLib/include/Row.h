#include <string>
#include <vector>
#include <initializer_list>
#include <any>
#include "GDefines.h"
#include <iostream>

#ifndef _ROW_H_
#define _ROW_H_

namespace pen
{
	template<class T>
	class Row
	{
		typedef T Entity;
	public:
		Row()
			:_Values()
		{}

		Row(const std::initializer_list<Entity>& list)
		{
			_Values.insert(_Values.end(), list.begin(), list.end());
		}

		Row(const std::vector<Entity>& other) : _Values(other){}
		Row(const Row& other) :_Values(other._Values){}
		Row(Row&& other) : _Values(std::move(other._Values)) {}

		~Row() {}

	public:

		Row& operator=(const Row& other)
		{
			_Values = other._Values;
		}

		Row& operator=(Row&& other)
		{
			_Values = std::move(other._Values);
		}

	public:

		inline int GetLength() const { return _Values.size(); }
		const Entity& EntityAt(const size_t x)
		{
			if (x >= 0 && x < GetLength()) {
				return _Values[x];
			}
			throw "Invalid Operation";
		}

		Entity& operator[](const int x)
		{
			if (x >= 0 && x < GetLength) {
				return _Values[x];
			}
			throw "Invalid Operation";
		}

#if _DEBUG_

		void Print()
		{
			std::cout << "---Row debug print---" << std::endl;
			for (int i = 0; i < GetLength(); i++)
			{
				std::cout << _Values[i] << std::endl;
			}
			std::cout << "---Row debug print---" << std::endl;
		}

#endif

	public:

		void SetAt(const size_t x, const Entity& value)
		{
			if (x >= 0 && x < GetLength()) {
				_Values[x] = value;
			}
			throw "Invalid Operation";
		}

		void SetAt(const size_t x, Entity&& value)
		{
			if (x >= 0 && x < GetLength()) {
				_Values[x] = std::move(value);
			}
			throw "Invalid Operation";
		}

	private:
		std::vector<Entity> _Values{};
	};
}

#endif // !_ROW_H_

