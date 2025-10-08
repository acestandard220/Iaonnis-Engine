#include "Environment.h"

namespace Iaonnis
{
	void Environment::load(filespace::filepath path)
	{
		std::ifstream file(path);
		if (!file.is_open())
		{
			IAONNIS_LOG_ERROR("Failed to read environment file.\n");
			return;
		}

		filespace::filepath parentPath = path.parent_path();
		std::vector<filespace::filepath> filePaths(6);
		int a = 0;
		std::string buffer;
		while (std::getline(file, buffer))
		{
			filespace::filepath mapPath = parentPath / buffer;
			if (!filespace::exists(mapPath))
			{
				IAONNIS_LOG_ERROR("Failed to find environment face map. (Path = %s)", buffer.c_str());
				return;
			}
			filePaths[a] = mapPath;
			IAONNIS_LOG_INFO("Successfully read environment face. (Path = %s)", buffer.c_str());
			a++;
		}

		file.close();

		TEXTURE_DESC desc[6];
		for (int i = 0; i < 6; i++)
		{
			if (stbi_is_16_bit(path.string().c_str()))
			{
				unsigned short* data = stbi_load_16(filePaths[i].string().c_str(), &width, &hieght, &nChannel, 0);

				desc[i].dataType = TEXTURE_DATA::TEXTURE_COLOR;
				desc[i].height = hieght;
				desc[i].width = width;
				desc[i].x = 0;
				desc[i].y = 0;
				desc[i].nBitPerChannel = 16;
				desc[i].nChannels = nChannel;
				desc[i].ptr = data;

				IAONNIS_LOG_WARN("Mem not being freed");
			}
			else
			{
				unsigned char* data = stbi_load(filePaths[i].string().c_str(), &width, &hieght, &nChannel, 0);

				desc[i].dataType = TEXTURE_DATA::TEXTURE_COLOR;
				desc[i].height = hieght;
				desc[i].width = width;
				desc[i].x = 0;
				desc[i].y = 0;
				desc[i].nBitPerChannel = 8;
				desc[i].nChannels = nChannel;
				desc[i].ptr = data;

				IAONNIS_LOG_WARN("Mem not being freed");
			}
		}

		handle = IGPUResource::createCubeMap(desc);
	}

	void Environment::save(filespace::filepath path)
	{
		std::ofstream file("Environment.iem"); //Iaonnis Environment Map(.iev)
		file << "No data written.\n";
		file.close();
	}
}
