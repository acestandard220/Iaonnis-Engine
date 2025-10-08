#pragma once
#include "Resource.h"
#include "../GPU/GPUResource.h"

namespace Iaonnis
{

	class ImageTexture : public Resource
	{
	public:
		ImageTexture();
		~ImageTexture();

		void load(filespace::filepath path) override;
		void save(filespace::filepath path) override;

		int getWidth() const { return width; }
		int getHeight() const { return height; }
		int getChannelCount() const { return nChannels; }
		int getBitPerChannel() const { return nBitPerChannel; }

		size_t GetBytSize()const { return (width * height * nChannels * nBitPerChannel)/8; }

		TextureHandle getTextureHandle() const { return handle; }

	private:

		int width;
		int height;
		int nChannels;
		int nBitPerChannel;

		TextureHandle handle;
		TEXTURE_DESC  desc;
	};

}