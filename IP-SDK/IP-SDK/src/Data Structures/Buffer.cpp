#include "Buffer.h"
#include <iostream>
using namespace sdk;

Buffer::Buffer(size_t nBufferSize) 
	:_nBufferSize(nBufferSize)
{}

Buffer::Buffer(const Buffer& other)
{
	this->_pData = other._pData;
	this->_nBufferSize = other._nBufferSize;
	this->_bIsWrapped = true;
}

Buffer::Buffer(Buffer&& other)
{
	this->_pData = other._pData;
	this->_nBufferSize = other._nBufferSize;
	this->_bIsWrapped = other._bIsWrapped;

	other._pData = nullptr;
}

Buffer::~Buffer() {
	if (_bIsWrapped == false && _pData != nullptr) {
		free(_pData);
	}
}

void* Buffer::GetData() {
	return _pData;
}

void Buffer::Create(void* pSrcData, bool bWrap, bool bSetZero) {
	if (_pData == nullptr && _nBufferSize > 0) {
		_bIsWrapped = bWrap;
		if (!_bIsWrapped) {
			_pData = (void*)malloc(_nBufferSize);
			if (_pData != nullptr) {
				if (pSrcData != nullptr) {
					memcpy(_pData, pSrcData, _nBufferSize);
				}
				else if (bSetZero) {
					memset(_pData, 0, _nBufferSize);
				}
			}
		}
		else {
			_pData = pSrcData;
		}
	}
}

Buffer* Buffer::DeepCopyPtr(const Buffer& other)
{
	Buffer* pCopy = new Buffer(other._nBufferSize);
	pCopy->_bIsWrapped = false;
	pCopy->Create();
	std::memcpy(pCopy->_pData, other._pData, pCopy->_nBufferSize);
	return pCopy;
}

Buffer Buffer::DeepCopy(const Buffer& other)
{
	Buffer buffer(other._nBufferSize);
	buffer._bIsWrapped = false;
	buffer.Create();
	std::memcpy(buffer._pData, other._pData, buffer._nBufferSize);
	return std::move(buffer);
}