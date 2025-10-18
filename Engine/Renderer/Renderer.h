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

		enum class SSBO_SLOT
		{
			DirectionalLight = 1,
			SpotLight = 2,
			PointLight = 3,

			Material = 4,
			CommandData = 5,
			Transform = 6,
			MaterialMap = 7
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

			int modelIndex;
		};

		void Initialize(uint32_t program);
		void Shutdown();
		
		void LockFence(GLsync& sync);
		void WaitFence(GLsync& sync);

		void UploadScene(Scene* scene);
		void UploadMaterialArray(Scene* scene);

		void UploadLightData(Scene* scene);
		void LightPass(Scene* scene);

		void SubmitDrawCommandData(DrawData data);
		void CloseDrawCommands();

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