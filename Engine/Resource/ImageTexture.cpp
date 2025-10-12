#include "ImageTexture.h"


namespace Iaonnis
{
	ImageTexture::ImageTexture()
	{
		type = ResourceType::ImageTexture;
	}

	ImageTexture::ImageTexture(const ImageTexture& other)
	{
		width = other.width;
		height = other.height;
		nChannels = other.nChannels;
		nBitPerChannel = other.nBitPerChannel;

		desc = other.desc;
		desc.ptr = nullptr;

		handle = IGPUResource::createGPUTexture(desc);
	}

	ImageTexture::~ImageTexture()
	{
		IGPUResource::destroyTexture(handle);
	}

	void ImageTexture::load(filespace::filepath path)
	{
		TEXTURE_DESC textureDesc;

		if (stbi_is_16_bit(path.string().c_str()))
		{
			unsigned short* data = stbi_load_16(path.string().c_str(), &width, &height, &nChannels, 0);
			if (!data)
			{
				IAONNIS_LOG_ERROR("Failed to load image file. (Path = %s)", path.string());
				return;
			}
			nBitPerChannel = 16;
			textureDesc.ptr = data;
		}
		else
		{
			unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &nChannels, 0);
			if (!data)
			{
				IAONNIS_LOG_ERROR("Failed to load image file. (Path = %s).", path.string());
				return;
			}
			nBitPerChannel = 8;
			textureDesc.ptr = data;
		}


		textureDesc.dataType = TEXTURE_DATA::TEXTURE_COLOR;
		textureDesc.height = height;
		textureDesc.width = width;
		textureDesc.nBitPerChannel = nBitPerChannel;
		textureDesc.nChannels = nChannels;
		textureDesc.x = 0;
		textureDesc.y = 0;

		handle = IGPUResource::createGPUTexture(textureDesc);
		desc = textureDesc;

	}
	
	void ImageTexture::save(filespace::filepath path)
	{
		IGPUResource::getTexturePixels(handle, desc);

		auto fileExtension = path.extension().string();
		int status = 0;

		if (fileExtension == ".png")
		{
			status = stbi_write_png(path.string().c_str(), desc.width, desc.height, desc.nChannels, desc.ptr, desc.width * desc.nChannels);
		}
		else if (fileExtension == ".jpg" || fileExtension == ".jpeg")
		{
			status = stbi_write_jpg(path.string().c_str(), desc.width, desc.height, desc.nChannels, desc.ptr, 100);
		}
		else if (fileExtension == ".bmp")
		{
			status = stbi_write_bmp(path.string().c_str(), desc.width, desc.height, desc.nChannels, desc.ptr);
		}
		else if (fileExtension == ".tga")
		{
			status = stbi_write_tga(path.string().c_str(), desc.width, desc.height, desc.nChannels, desc.ptr);
		}

		free(desc.ptr);
		if (!status)
		{
			IAONNIS_LOG_ERROR("Failed to write Image File.");
			return;
		}
	}
}