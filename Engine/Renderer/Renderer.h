#pragma once
#include "../Core/Core.h"
#include "../Resource/Mesh.h"
#include "../Scene/Scene.h"
#include "../Scene/Components.h"
#include "../Scene/Entity.h"

namespace Iaonnis
{
	namespace Renderer3D
	{
		struct RendererStatistics
		{
			size_t nDrawCalls = 0;
			size_t nRenderedVertices = 0;
			size_t nRenderedIndices = 0;

			size_t totalTextureBufferSize = 0;

			//Time Stamps
			double sceneUploadTime = 0;
			double materialUploadTime = 0;
			double lightUploadTime = 0;

		};

		enum SSBO_SLOT
		{
			Diffuse = 0,
			Normal = 1,

			DirectionalLight = 4,
			SpotLight = 5,
			PointLight = 6,

			AO = 7, 
			Metallic = 8,
			Roughness = 9
		};

		enum class gBufferHandles
		{
			Albedo,Position,Normal,AO,Roughness,Metallic,Depth
		};

		struct DrawData
		{
			void* vertexPtr;
			void* indexPtr;

			int vertexCount;
			int indexCount;
		};

		void Initialize(uint32_t program);
		void Shutdown();
		
		void EnvironmentPass(Scene* scene);

		void UploadScene(Scene* scene);
		void UploadSceneNoBatching(Scene* scene);
		void UploadMaterialArray(Scene* scene);
		void UploadMaterialsToGPU();

		void UploadLightData(Scene* scene);
		void LightPass(Scene* scene);

		void submitDrawCommandData(DrawData data);
		void closeDrawCommands();

		void drawCommands(Scene* scene, uint32_t program);
		void resetGeometryPtrs();
		void resetLightPtrs();
		void resetMaterialPtrs();

		void RenderScene(Scene* scene, uint32_t program);

		//Temporary Output.
		uint32_t GetRenderOutput();


		void OnViewFrameResize(Event& event);

		RendererStatistics GetRenderStats();

	}
}