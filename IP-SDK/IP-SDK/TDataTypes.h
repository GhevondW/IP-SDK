#ifndef __T_DATA_TYPES_H__
#define __T_DATA_TYPES_H__

#define __validate_value(x, min, max) (x >= min && x < max)

namespace sdk
{

	static const char DT4_SIZE = 4;
	static const char DT3_SIZE = 3;

	template<class DATA_TYPE>
	class _data_type4
	{
	public:
		_data_type4(DATA_TYPE x, DATA_TYPE y, DATA_TYPE z, DATA_TYPE w)
		{
			_data[0] = x;
			_data[1] = y;
			_data[2] = z;
			_data[3] = w;
		}
		~_data_type4() 
		{
		
		}

		DATA_TYPE& operator[](int index) {
			if (__validate_value(index, 0, DT4_SIZE)) {
				return _data[index];
			}
			return _data[0];
		}

		const DATA_TYPE at(const int index) const
		{
			if (__validate_value(index, 0, DT4_SIZE)) {
				return _data[index];
			}
			return -1;
		}

	private:
		DATA_TYPE _data[DT4_SIZE] = {0};
	};

	template<class DATA_TYPE>
	class _data_type3
	{
	public:
		_data_type3(DATA_TYPE x, DATA_TYPE y, DATA_TYPE z, DATA_TYPE w)
		{
			_data[0] = x;
			_data[1] = y;
			_data[2] = z;
			_data[3] = w;
		}
		~_data_type3()
		{

		}

		DATA_TYPE& operator[](int index) {
			if (__validate_value(index, 0, DT3_SIZE)) {
				return _data[index];
			}
			return _data[0];
		}

		const DATA_TYPE at(const int index) const
		{
			if (__validate_value(index, 0, DT3_SIZE)) {
				return _data[index];
			}
			return -1;
		}

	private:
		DATA_TYPE _data[DT3_SIZE] = { 0 };
	};

}

#endif // !__T_DATA_TYPES_H__

