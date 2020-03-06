#include "Image.h"
using namespace sdk;

Image::~Image()
{
	if (_pBuffer != nullptr) {
		delete _pBuffer;
	}
}

Image::Image(const Image& other)
{
	_bIsWrapped = true;
	_pBuffer = other._pBuffer;
	_nWidth = other._nWidth;
	_nHeight = other._nHeight;
	_nRowByte = other._nRowByte;
	_nBpp = other._nBpp;
	_nComponentCount = other._nComponentCount;
}

Image::Image(Image&& other)
{
	_bIsWrapped = other._bIsWrapped;
	_pBuffer = other._pBuffer;
	_nWidth = other._nWidth;
	_nHeight = other._nHeight;
	_nRowByte = other._nRowByte;
	_nBpp = other._nBpp;
	_nComponentCount = other._nComponentCount;

	other._pBuffer = nullptr;
}

void Image::SetRowByte()
{
	_nRowByte = _nBpp * _nComponentCount * _nWidth;
}

void Image::Create(bool setZero)
{
	if (_pBuffer == nullptr) {
		size_t nBufferSize = _nRowByte * _nHeight;
		_pBuffer = new Buffer(nBufferSize);
		_pBuffer->Create(setZero);
	}
}

//bulder

ImageBuilder::ImageBuilder()
{
	_pImage = new Image();
}

ImageBuilderInterface& ImageBuilder::SetWidth(size_t value) 
{
	_pImage->_nWidth = value;
	return *this;
}

ImageBuilderInterface& ImageBuilder::SetHeight(size_t value) 
{
	_pImage->_nHeight = value;
	return *this;
}

ImageBuilderInterface& ImageBuilder::SetBpp(size_t value) 
{
	_pImage->_nBpp = value;
	return *this;
}

ImageBuilderInterface& ImageBuilder::SetComponenetCount(size_t value) 
{
	_pImage->_nComponentCount = value;
	return *this;
}

ImageBuilderInterface& ImageBuilder::SetZero(bool value) 
{
	_bSetZero = value;
	return *this;
}

Image* ImageBuilder::Build() 
{
	if (_pImage != nullptr) {
		_pImage->SetRowByte();
		_pImage->Create(_bSetZero);
		return _pImage;
	}
	return nullptr;
}