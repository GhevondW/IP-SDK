#ifndef _RECT_H_
#define _RECT_H_

#include "Point.h"

namespace sdk
{
	class Rect
	{
	public:
		Rect()
		{}
		Rect(int top, int bottom, int left, int right)
			:_iTop(top),
			_iBottom(bottom),
			_iLeft(left),
			_iRight(right)
		{}
		~Rect() {}

	public:

		inline int GetTop() const { return _iTop; }
		inline int GetLeft() const { return _iLeft; }
		inline int GetBottom() const { return _iBottom; }
		inline int GetRight() const { return _iRight; }

	public:

		inline void SetTop(int value) { _iTop = value; }
		inline void SetLeft(int value) { _iLeft = value; }
		inline void SetBottom(int value) { _iBottom = value; }
		inline void SetRight(int value) { _iRight = value; }

	public:

		bool Contains(sdk::Point point)
		{
			int x = point.GetX();
			int y = point.GetY();
			return x > _iLeft&& x < _iRight && y > _iTop&& y < _iBottom;
		}

	private:
		int _iTop{0};
		int _iBottom{0};
		int _iLeft{0};
		int _iRight{0};
	};
};

#endif // !_RECT_H_

