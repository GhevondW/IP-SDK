#ifndef _POINT_H_
#define _POINT_H_

namespace sdk
{

	class Point
	{
	public:
		Point() {}
		Point(int x, int y)
			:_iX(x),
			_iY(y) 
		{}

		~Point() {}

	public:

		inline int GetX() const { return _iX; }
		inline int GetY() const { return _iY; }

	public:

		inline void SetX(int value) { _iX = value; }
		inline void SetY(int value) { _iY = value; }

	private:
		int _iX{};
		int _iY{};
	};

};

#endif // !_POINT_H_

