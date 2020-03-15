#include <string>
#include <vector>
#include <initializer_list>
#include <any>

#ifndef _ROW_H_
#define _ROW_H_

namespace pen
{
	class Row
	{
		typedef std::any Entity;
	public:
		Row();
		Row(std::initializer_list<std::any>);
		Row(const std::vector<Entity>& other);
		Row(const Row&);
		Row(Row&&);

		~Row();

	public:

		Row& operator=(const Row& other);
		Row& operator=(Row&& other);

	public:

		int GetLength() const;
		const Entity& EntityAt(const size_t x);

		template<class T>
		const T& ValueAt(const size_t x);

	public:

		void SetAt(const size_t x, const std::any& value);

	private:
		std::vector<Entity> _Values{};
	};
}

#endif // !_ROW_H_

