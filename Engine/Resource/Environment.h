#pragma once
#include "Resource.h"
#include "../GPU/GPUResource.h"

namespace Iaonnis
{
	class Environment : public Resource
	{
	public:
		Environment() = default;
		~Environment() = default;

		void load(filespace::filepath path) override;
		void save(filespace::filepath path) override;


		CubeMapHandle GetCubeMapHandle() { return handle; }

	private:
		int width;
		int hieght;
		int nChannel;
		int bitPerChannel;

		CubeMapHandle handle;
	};

}