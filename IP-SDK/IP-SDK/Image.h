#pragma once

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "Buffer.h"



namespace sdk
{
	
	class Image;

	class ImageBuilderInterface
	{
	public:
		virtual ImageBuilderInterface& SetWidth(size_t value) = 0;
		virtual ImageBuilderInterface& SetHeight(size_t value) = 0;
		virtual ImageBuilderInterface& SetBpp(size_t value) = 0;
		virtual ImageBuilderInterface& SetComponenetCount(size_t value) = 0;
		virtual ImageBuilderInterface& SetZero(bool value) = 0;
		virtual ImageBuilderInterface& SetSrcData(void* value) = 0;
		virtual ImageBuilderInterface& Wrap() = 0;
		virtual Image* Build() = 0;
	};

	class ImageBuilder : public ImageBuilderInterface
	{
	public:
		ImageBuilder();
		ImageBuilderInterface& SetWidth(size_t value) override;
		ImageBuilderInterface& SetHeight(size_t value) override;
		ImageBuilderInterface& SetBpp(size_t value) override;
		ImageBuilderInterface& SetComponenetCount(size_t value) override;
		ImageBuilderInterface& SetZero(bool value) override;
		ImageBuilderInterface& SetSrcData(void* value) override;
		ImageBuilderInterface& Wrap() override;
		Image* Build() override;
	private:
		Image* _pImage{ nullptr };
		bool _bSetZero;
	};

	class Image
	{
	public:
		friend class ImageBuilder;

		static ImageBuilder Make()
		{
			return ImageBuilder();
		}

		~Image();

		Image(const Image& other);
		Image(Image&& other);

	private:
		Image() = default;
		void Create(bool setZero = false);
		void SetRowByte();

	private:
		Buffer* _pBuffer{ nullptr };
		void* _pSrcData{ nullptr };
		size_t _nWidth{ 0 };
		size_t _nHeight{ 0 };
		size_t _nBpp{ 0 };
		size_t _nComponentCount{ 0 };
		size_t _nRowByte{ 0 };
		size_t _bIsWrapped{ false };
	};

	
	
};

#endif // !__IMAGE_H__



