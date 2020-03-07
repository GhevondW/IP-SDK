#ifndef __BUFFER_H__
#define __BUFFER_H__


#include <malloc.h>

namespace sdk
{

	class Buffer
	{
	public:

		Buffer(size_t);
		Buffer(const Buffer& other);
		Buffer(Buffer&& other);
		~Buffer();

		Buffer* DeepCopyPtr(const Buffer& other);
		Buffer DeepCopy(const Buffer& other);
		void* GetData();
		void Create(void* pSrcDara = nullptr, bool bSetZero = false);

	private:
		void* _pData{nullptr};
		size_t _nBufferSize{0};
		bool _bIsWrapped{false};
	};

}

#endif // !__BUFFER_H__


