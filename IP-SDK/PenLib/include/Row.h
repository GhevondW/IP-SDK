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
		friend class Table;
	public:
		Row()
			:_Values(),
			_pColRef(nullptr)
		{}

		Row(std::initializer_list<T>&& list, const Row<std::string>* colRef = nullptr)
			:_pColRef(colRef)
		{
			_Values.insert(_Values.end(), list.begin(), list.end());
		}

		Row(const std::vector<Entity>& other, const Row<std::string>* colRef = nullptr) : _Values(other), _pColRef(colRef){}
		Row(const Row& other) :_Values(other._Values)
		{
			_pColRef = other._pColRef;
		}
		Row(Row&& other) : _Values(std::move(other._Values)) 
		{
			_pColRef = other._pColRef;
			other._pColRef = nullptr;
		}

		~Row() 
		{
			_pColRef = nullptr;
		}

	public:

		Row& operator=(const Row& other)
		{
			_Values = other._Values;
			return *this;
		}

		Row& operator=(Row&& other)
		{
			_Values = std::move(other._Values);
			return *this;
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
			if (x >= 0 && x < GetLength()) {
				return _Values[x];
			}
			throw "Invalid Operation";
		}

		Entity& LocCol(const std::string& colName) 
		{
			if (_pColRef != nullptr) {
				auto it = std::find(_pColRef->_Values.begin(), _pColRef->_Values.end(), colName);
				if (it != _pColRef->_Values.end()) {
					int index = std::distance(_pColRef->_Values.begin(), it);
					int size = GetLength();
					if (index >= 0 && index < size) {
						return _Values[index];
					}
				}
			}
			throw "invalid operation";
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

		void PushBack(const Entity& value)
		{
			_Values.push_back(value);
		}

	private:
		std::vector<Entity> _Values{};
		const Row<std::string>* _pColRef{nullptr};
	};
}

#endif // !_ROW_H_

