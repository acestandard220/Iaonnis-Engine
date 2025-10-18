#include "Renderer.h"
#include "RendererResources.h"

namespace Iaonnis {
	namespace Renderer3D {

		struct DirectionalLightUpload
		{
			glm::vec4 direction;
			glm::vec4 color;
		};

		struct SpotLightUpload
		{
			glm::vec4 position; //w = inner Radiuse
			glm::vec4 color;    //w = outer Radius
			glm::vec4 direction;
		};

		struct PointLightUpload
		{
			glm::vec4 position;
			glm::vec4 color;
		};

		struct LightMeta
		{
			glm::vec4 viewPos; //Note: ViewPos will change based on the mode of render.
			int nDirectionLight = 0;
			int nSpotLight = 0;
			int nPointLight = 0;
			int padding;
		};

		struct DrawElementsIndirectCommand {
			uint32_t  count;
			uint32_t  instanceCount;
			uint32_t  firstIndex;
			int  baseVertex;
			uint32_t  baseInstance;
		};

		struct MaterialUpload {
			GLuint64 albedo;
			GLuint64 normal;
			GLuint64 ao;
			GLuint64 roughness;
			GLuint64 metal;

			GLuint64 _pad1;
			
			glm::vec4 color;
			glm::vec4 uvScale; //z = normalStrength & w = flipNormalY
		};

		struct CascadeMatrixSet
		{
			glm::mat4 mat[4];
		};

		struct CommandData
		{
			int offset = 0;

			int nMeshes = 0;
		};

		struct RendererData{
			glm::vec2 frameSize{ 800,800 };

			uint32_t vao;
			uint32_t vbo;
			uint32_t ebo;
			uint32_t ibo;

			void* vboPtr;
			void* eboPtr;
			void* iboPtr;
			GLsync gSync;

			uint32_t commandDataSSBO;
			void* commandDataBufferPtr;
			int commnadDataBufferOffset = 0;
			int subMeshOffset = 0;

			uint32_t transformSSBO;
			glm::mat4* transformBufferPtr;


			uint32_t materialSSBO;
			uint32_t materialMapSSBO;
			void* materialBufferPtr;
			int* materialMapBufferPtr;

			uint32_t screenQuadVao;
			uint32_t screenQuadVbo;
			uint32_t screenQuadEbo;

			uint32_t gbufferProgram;
			uint32_t lightProgram;
			uint32_t depthProgram;

			uint32_t lightSpaceMatrixUBO;

			uint32_t directionalLightSSBO;
			uint32_t spotLigtSSBO;
			uint32_t pointLigtSSBO;
			uint32_t lightMetaUBO;

			FramebufferHandle lightPassFBO;
			FramebufferHandle gBuffer;


			static const int MAX_VERTEX = 10000000;
			static const int MAX_INDICES = 15000000;
			static const int MAX_DRAW_COMMANDS = 300;
			static const int MAX_MATERIALS = 300;
			static const int MAX_TYPE_OF_LIGHT = 300;
			static const int MAX_SUBMESHES = MAX_DRAW_COMMANDS * 100;

			int commandPtr = 0;

			int indexCount = 0;
			int vertexCount = 0;
			int currentVertexCount = 0;
			int currentIndexCount = 0;

			std::unordered_map<UUID, int> materialMapCache;
			MaterialUpload materialUploadArr[MAX_MATERIALS];
			int materialUploadPtr = 0;

			DirectionalLightUpload directionalLightArr[MAX_TYPE_OF_LIGHT];
			SpotLightUpload spotLightArr[MAX_TYPE_OF_LIGHT];
			PointLightUpload pointLightArr[MAX_TYPE_OF_LIGHT];

			CascadeMatrixSet lightSpaceMatrixArr[MAX_TYPE_OF_LIGHT];
			int lightSpaceMatrixPtr = 0;

			LightMeta lightMeta{};


			uint32_t depthFBO;
			uint32_t depthTexture;

			glm::mat4 depthView;
			glm::mat4 depthProjection;

		}rendererData;

		RendererStatistics RendererStats{};


		void CreateLightBuffers()
		{
			glGenBuffers(1, &rendererData.directionalLightSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.directionalLightSSBO);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(DirectionalLightUpload) * rendererData.MAX_TYPE_OF_LIGHT, nullptr, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (int)SSBO_SLOT::DirectionalLight, rendererData.directionalLightSSBO);

			glGenBuffers(1, &rendererData.spotLigtSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.spotLigtSSBO);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(SpotLightUpload) * rendererData.MAX_TYPE_OF_LIGHT, nullptr, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (int)SSBO_SLOT::SpotLight, rendererData.spotLigtSSBO);

			glGenBuffers(1, &rendererData.pointLigtSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.pointLigtSSBO);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(PointLightUpload) * rendererData.MAX_TYPE_OF_LIGHT, nullptr, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (int)SSBO_SLOT::PointLight, rendererData.pointLigtSSBO);

			glGenBuffers(1, &rendererData.lightMetaUBO);
			glBindBuffer(GL_UNIFORM_BUFFER, rendererData.lightMetaUBO);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(LightMeta), nullptr, GL_DYNAMIC_DRAW);
			glBindBufferRange(GL_UNIFORM_BUFFER, 2, rendererData.lightMetaUBO, 0, sizeof(LightMeta));
			glUniformBlockBinding(rendererData.lightProgram, glGetUniformBlockIndex(rendererData.lightProgram, "LightMeta"), 2);
		}

		void CreateIndirectDrawBuffers()
		{
			glGenVertexArrays(1, &rendererData.vao);
			glBindVertexArray(rendererData.vao);

			glGenBuffers(1, &rendererData.vbo);
			glGenBuffers(1, &rendererData.ebo);
			glGenBuffers(1, &rendererData.ibo);

			glBindBuffer(GL_ARRAY_BUFFER, rendererData.vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererData.ebo);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, rendererData.ibo);

			GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
			
			glBufferStorage(GL_ARRAY_BUFFER, sizeof(Vertice) * rendererData.MAX_VERTEX, nullptr, flags);
			rendererData.vboPtr = glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(Vertice) * rendererData.MAX_VERTEX, flags);

			glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * rendererData.MAX_INDICES, nullptr, flags);
			rendererData.eboPtr = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * rendererData.MAX_INDICES, flags);

			glBufferStorage(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand) * rendererData.MAX_DRAW_COMMANDS, nullptr, flags);
			rendererData.iboPtr = glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(DrawElementsIndirectCommand) * rendererData.MAX_DRAW_COMMANDS, flags);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), 0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, n));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, uv));
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, tangent));
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, bitangent));
			glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT, sizeof(Vertice), (void*)offsetof(Vertice, id));

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
			glEnableVertexAttribArray(4);
			glEnableVertexAttribArray(5);

			glBindVertexArray(0);

		}

		void CreateGBuffer()
		{
			//====================G-BUFFER================================
			FRAMEBUFFER_DESC gBufferDesc;
			gBufferDesc.n_Desc = 7;
			gBufferDesc.textureDesc = (TEXTURE_DESC*)malloc(sizeof(TEXTURE_DESC) * gBufferDesc.n_Desc);
			gBufferDesc.textureDesc[(int)gBufferHandles::Albedo].dataType = TEXTURE_DATA::TEXTURE_COLOR;
			gBufferDesc.textureDesc[(int)gBufferHandles::Albedo].height = 800;
			gBufferDesc.textureDesc[(int)gBufferHandles::Albedo].width = 800;
			gBufferDesc.textureDesc[(int)gBufferHandles::Albedo].nBitPerChannel = 32;
			gBufferDesc.textureDesc[(int)gBufferHandles::Albedo].nChannels = 4;
			gBufferDesc.textureDesc[(int)gBufferHandles::Albedo].ptr = nullptr;
			gBufferDesc.textureDesc[(int)gBufferHandles::Albedo].x = 0;
			gBufferDesc.textureDesc[(int)gBufferHandles::Albedo].y = 0;

			gBufferDesc.textureDesc[(int)gBufferHandles::Position].dataType = TEXTURE_DATA::TEXTURE_COLOR;
			gBufferDesc.textureDesc[(int)gBufferHandles::Position].height = 800;
			gBufferDesc.textureDesc[(int)gBufferHandles::Position].width = 800;
			gBufferDesc.textureDesc[(int)gBufferHandles::Position].nBitPerChannel = 32;
			gBufferDesc.textureDesc[(int)gBufferHandles::Position].nChannels = 4;
			gBufferDesc.textureDesc[(int)gBufferHandles::Position].ptr = nullptr;
			gBufferDesc.textureDesc[(int)gBufferHandles::Position].x = 0;
			gBufferDesc.textureDesc[(int)gBufferHandles::Position].y = 0;

			gBufferDesc.textureDesc[(int)gBufferHandles::Normal].dataType = TEXTURE_DATA::TEXTURE_COLOR;
			gBufferDesc.textureDesc[(int)gBufferHandles::Normal].height = 800;
			gBufferDesc.textureDesc[(int)gBufferHandles::Normal].width = 800;
			gBufferDesc.textureDesc[(int)gBufferHandles::Normal].nBitPerChannel = 32;
			gBufferDesc.textureDesc[(int)gBufferHandles::Normal].nChannels = 4;
			gBufferDesc.textureDesc[(int)gBufferHandles::Normal].ptr = nullptr;
			gBufferDesc.textureDesc[(int)gBufferHandles::Normal].x = 0;
			gBufferDesc.textureDesc[(int)gBufferHandles::Normal].y = 0;

			gBufferDesc.textureDesc[(int)gBufferHandles::AO].dataType = TEXTURE_DATA::TEXTURE_COLOR;
			gBufferDesc.textureDesc[(int)gBufferHandles::AO].height = 800;
			gBufferDesc.textureDesc[(int)gBufferHandles::AO].width = 800;
			gBufferDesc.textureDesc[(int)gBufferHandles::AO].nBitPerChannel = 32;
			gBufferDesc.textureDesc[(int)gBufferHandles::AO].nChannels = 1;
			gBufferDesc.textureDesc[(int)gBufferHandles::AO].ptr = nullptr;
			gBufferDesc.textureDesc[(int)gBufferHandles::AO].x = 0;
			gBufferDesc.textureDesc[(int)gBufferHandles::AO].y = 0;

			gBufferDesc.textureDesc[(int)gBufferHandles::Roughness].dataType = TEXTURE_DATA::TEXTURE_COLOR;
			gBufferDesc.textureDesc[(int)gBufferHandles::Roughness].height = 800;
			gBufferDesc.textureDesc[(int)gBufferHandles::Roughness].width = 800;
			gBufferDesc.textureDesc[(int)gBufferHandles::Roughness].nBitPerChannel = 32;
			gBufferDesc.textureDesc[(int)gBufferHandles::Roughness].nChannels = 1;
			gBufferDesc.textureDesc[(int)gBufferHandles::Roughness].ptr = nullptr;
			gBufferDesc.textureDesc[(int)gBufferHandles::Roughness].x = 0;
			gBufferDesc.textureDesc[(int)gBufferHandles::Roughness].y = 0;

			gBufferDesc.textureDesc[(int)gBufferHandles::Metallic].dataType = TEXTURE_DATA::TEXTURE_COLOR;
			gBufferDesc.textureDesc[(int)gBufferHandles::Metallic].height = 800;
			gBufferDesc.textureDesc[(int)gBufferHandles::Metallic].width = 800;
			gBufferDesc.textureDesc[(int)gBufferHandles::Metallic].nBitPerChannel = 32;
			gBufferDesc.textureDesc[(int)gBufferHandles::Metallic].nChannels = 1;
			gBufferDesc.textureDesc[(int)gBufferHandles::Metallic].ptr = nullptr;
			gBufferDesc.textureDesc[(int)gBufferHandles::Metallic].x = 0;
			gBufferDesc.textureDesc[(int)gBufferHandles::Metallic].y = 0;

			gBufferDesc.textureDesc[(int)gBufferHandles::Depth].dataType = TEXTURE_DATA::TEXTURE_DEPTH;
			gBufferDesc.textureDesc[(int)gBufferHandles::Depth].height = 800;
			gBufferDesc.textureDesc[(int)gBufferHandles::Depth].width = 800;
			gBufferDesc.textureDesc[(int)gBufferHandles::Depth].nBitPerChannel = 24;
			gBufferDesc.textureDesc[(int)gBufferHandles::Depth].nChannels = 1;
			gBufferDesc.textureDesc[(int)gBufferHandles::Depth].ptr = nullptr;
			gBufferDesc.textureDesc[(int)gBufferHandles::Depth].x = 0;
			gBufferDesc.textureDesc[(int)gBufferHandles::Depth].y = 0;

			rendererData.gBuffer = IGPUResource::createFrambuffer(gBufferDesc, FBO_FLAGS::ALL_COLOR_ATTACHMENT);
			free(gBufferDesc.textureDesc);

		}

		void CreateMaterialSSBO()
		{
			glGenBuffers(1, &rendererData.materialSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.materialSSBO);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (int)SSBO_SLOT::Material, rendererData.materialSSBO);

			GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
			glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(MaterialUpload) * rendererData.MAX_MATERIALS, nullptr, flags);
			rendererData.materialBufferPtr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(MaterialUpload) * rendererData.MAX_MATERIALS, flags);

			glGenBuffers(1, &rendererData.materialMapSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.materialMapSSBO);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (int)SSBO_SLOT::MaterialMap, rendererData.materialMapSSBO);

			glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(int) * rendererData.MAX_SUBMESHES, nullptr, flags);
			rendererData.materialMapBufferPtr = (int*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int) * rendererData.MAX_SUBMESHES, flags);
		}

		void CreateShaders()
		{
			rendererData.gbufferProgram = CreateShaderProgram("Assets/Shaders/vertex.glsl", "Assets/Shaders/fragment.glsl");;
			rendererData.lightProgram = CreateShaderProgram("Assets/Shaders/lightVert.glsl", "Assets/Shaders/pbrFragment.glsl", nullptr);
			rendererData.depthProgram = CreateShaderProgram("Assets/Shaders/depthVert.glsl", "Assets/Shaders/depthFrag.glsl", nullptr);
		}

		void Iaonnis::Renderer3D::Initialize(uint32_t program)
		{
			SCOPE_TIMER(__FUNCTION__);

			CreateShaders();
			CreateIndirectDrawBuffers();
		
			CreateMaterialSSBO();
			CreateLightBuffers();

			
			glGenBuffers(1, &rendererData.commandDataSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.commandDataSSBO);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (int)SSBO_SLOT::CommandData, rendererData.commandDataSSBO);

			GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
			glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(CommandData) * rendererData.MAX_DRAW_COMMANDS, nullptr, flags);
			rendererData.commandDataBufferPtr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(CommandData) * rendererData.MAX_DRAW_COMMANDS, flags);
			
			glGenBuffers(1, &rendererData.transformSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.transformSSBO);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (int)SSBO_SLOT::Transform, rendererData.transformSSBO);

			glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * rendererData.MAX_DRAW_COMMANDS, nullptr, flags);
			rendererData.transformBufferPtr = (glm::mat4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * rendererData.MAX_DRAW_COMMANDS, flags);


			glGenBuffers(1, &rendererData.lightSpaceMatrixUBO);
			glBindBuffer(GL_UNIFORM_BUFFER, rendererData.lightSpaceMatrixUBO);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(CascadeMatrixSet), nullptr, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, rendererData.lightSpaceMatrixUBO);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			//=====================ScrenQuad Buffers===============================
			float screenQuadVertices[16] =
			{
				-1.0,-1.0,0.0,0.0,
				 1.0,-1.0,1.0,0.0,
				 1.0, 1.0,1.0,1.0,
				-1.0, 1.0,0.0,1.0
			};

			uint32_t screenQuadIndices[6] =
			{
				0,1,2,
				0,2,3
			};

			glGenVertexArrays(1, &rendererData.screenQuadVao);
			glBindVertexArray(rendererData.screenQuadVao);

			glGenBuffers(1, &rendererData.screenQuadVbo);
			glBindBuffer(GL_ARRAY_BUFFER, rendererData.screenQuadVbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, screenQuadVertices, GL_STATIC_DRAW);

			glGenBuffers(1, &rendererData.screenQuadEbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererData.screenQuadEbo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 6, screenQuadIndices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2));

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);

			glBindVertexArray(0);
			//=====================================================================
			// 
			//====================FBO================================
			FRAMEBUFFER_DESC fboDesc;
			fboDesc.n_Desc = 3;
			fboDesc.textureDesc = (TEXTURE_DESC*)malloc(sizeof(TEXTURE_DESC) * fboDesc.n_Desc);
			fboDesc.textureDesc[0].dataType = TEXTURE_DATA::TEXTURE_COLOR;
			fboDesc.textureDesc[0].height = 800;
			fboDesc.textureDesc[0].width = 800;
			fboDesc.textureDesc[0].nBitPerChannel = 32;
			fboDesc.textureDesc[0].nChannels = 4;
			fboDesc.textureDesc[0].ptr = nullptr;
			fboDesc.textureDesc[0].x = 0;
			fboDesc.textureDesc[0].y = 0;

			fboDesc.textureDesc[1].dataType = TEXTURE_DATA::TEXTURE_COLOR;
			fboDesc.textureDesc[1].height = 800;
			fboDesc.textureDesc[1].width = 800;
			fboDesc.textureDesc[1].nBitPerChannel = 32;
			fboDesc.textureDesc[1].nChannels = 4;
			fboDesc.textureDesc[1].ptr = nullptr;
			fboDesc.textureDesc[1].x = 0;
			fboDesc.textureDesc[1].y = 0;

			fboDesc.textureDesc[2].dataType = TEXTURE_DATA::TEXTURE_DEPTH;
			fboDesc.textureDesc[2].height = 800;
			fboDesc.textureDesc[2].width = 800;
			fboDesc.textureDesc[2].nBitPerChannel = 24;
			fboDesc.textureDesc[2].nChannels = 1;
			fboDesc.textureDesc[2].ptr = nullptr;
			fboDesc.textureDesc[2].x = 0;
			fboDesc.textureDesc[2].y = 0;

			rendererData.lightPassFBO = IGPUResource::createFrambuffer(fboDesc, FBO_FLAGS::ALL_COLOR_ATTACHMENT);
			free(fboDesc.textureDesc);

			CreateGBuffer();


			glGenFramebuffers(1, &rendererData.depthFBO);
			glBindFramebuffer(GL_FRAMEBUFFER, rendererData.depthFBO);

			glGenTextures(1, &rendererData.depthTexture);
			glBindTexture(GL_TEXTURE_2D, rendererData.depthTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

			GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rendererData.depthTexture, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);


			rendererData.depthView = glm::lookAt(glm::vec3(3.0f, 3.0f, 8.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			rendererData.depthProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 25.0f);
			

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				std::cerr << "Depth framebuffer not complete!" << std::endl;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			//=====================================
			rendererData.commandPtr = 0;
			rendererData.indexCount = 0;
			rendererData.vertexCount = 0;
			rendererData.currentIndexCount = 0;
			//=====================================

			EventBus::subscribe(EventType::RESIZE_EVENT, OnViewFrameResize);
		}

		void RenderShadowMap(Scene* scene)
		{
			auto camera = scene->GetSceneCamera();
			//rendererData.depthView = camera->getView();
			//rendererData.depthProjection = camera->getProjection();

			rendererData.depthView = glm::lookAt(glm::vec3(rendererData.pointLightArr[0].position), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
			//rendererData.depthProjection = glm::perspective(glm::radians(rendererData.spotLightArr[0].position.w), 1.0f, 1.0f, 10000.0f);

			glViewport(0, 0, 1024, 1024);
			glBindFramebuffer(GL_FRAMEBUFFER, rendererData.depthFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			glUseProgram(rendererData.depthProgram);

			int location = glGetUniformLocation(rendererData.depthProgram, "view");
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(rendererData.depthView));

			location = glGetUniformLocation(rendererData.depthProgram, "projection");
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(rendererData.depthProjection));

			glCullFace(GL_FRONT);
			drawCommands(scene, rendererData.depthProgram);
			glCullFace(GL_BACK);
			glViewport(0, 0, rendererData.frameSize.x, rendererData.frameSize.y);
		}

		void Shutdown()
		{
			SCOPE_TIMER(__FUNCTION__);

			glDeleteVertexArrays(1, &rendererData.vao);
			glDeleteBuffers(1, &rendererData.vbo);
			glDeleteBuffers(1, &rendererData.ebo);
			glDeleteBuffers(1, &rendererData.ibo);

			glUnmapBuffer(GL_ARRAY_BUFFER);
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
			glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);

			glDeleteVertexArrays(1, &rendererData.screenQuadVao);
			glDeleteBuffers(1, &rendererData.screenQuadVbo);
			glDeleteBuffers(1, &rendererData.screenQuadEbo);

			glDeleteProgram(rendererData.lightProgram);

			glDeleteBuffers(1, &rendererData.directionalLightSSBO);
			glDeleteBuffers(1, &rendererData.spotLigtSSBO);
			glDeleteBuffers(1, &rendererData.pointLigtSSBO);

			glDeleteBuffers(1, &rendererData.lightMetaUBO);


			IGPUResource::DeleteFramebuffer(rendererData.gBuffer);
			IGPUResource::DeleteFramebuffer(rendererData.lightPassFBO);

			IAONNIS_LOG_INFO("Renderer has shutdown");
		}

		void LockFence(GLsync& sync)
		{
			if (sync)
			{
				glDeleteSync(sync);
			}

			sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		}

		void WaitFence(GLsync& sync)
		{
			if (sync)
			{
				while (true)
				{
					GLenum wait = glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
					if (wait == GL_ALREADY_SIGNALED || wait == GL_CONDITION_SATISFIED)
						break;
				}
			}
		}

		void UploadScene(Scene* scene)
		{
			resetGeometryPtrs();

			std::shared_ptr<ResourceCache> cache = scene->getCache();
			std::vector<std::shared_ptr<Material>> materials = cache->getByType<Material>(ResourceType::Material);
			std::vector<std::shared_ptr<Mesh>> meshes = cache->getByType<Mesh>(ResourceType::Mesh);
			auto meshEntities = scene->getEntitiesWith<MeshFilterComponent>();

			//-------------------------------------------------------------
			for (auto& mesh : meshes)
			{
				auto meshID = mesh->GetID();
				int subMeshCount = mesh->getSubMeshCount();

				for (auto meshEntity : meshEntities)
				{
					auto& meshFilter = meshEntity.GetComponent<MeshFilterComponent>();
					auto transform = meshEntity.GetTransformMatrix();

					if (meshFilter.meshID != meshID || !meshEntity.active)
						continue;

					for (int i = 0; i < subMeshCount; i++)
					{
						SubMesh* subMesh = mesh->getSubMesh(i);

						DrawData data;
						data.indexCount = subMesh->indexCount;
						data.indexPtr = mesh->getSubMeshIndexStart(i);

						data.vertexCount = subMesh->vertexCount;
						data.vertexPtr = mesh->getSubMeshVerticeStart(i);

						SubmitDrawCommandData(data);

						rendererData.materialMapBufferPtr[rendererData.subMeshOffset + i] = rendererData.materialMapCache[meshEntity.GetSubMeshMaterial(i)];
					}

					rendererData.transformBufferPtr[rendererData.commandPtr] = transform;
					CloseDrawCommands();
				}
			}
			rendererData.subMeshOffset = 0;
		}

		void Renderer3D::UploadMaterialArray(Scene* scene)
		{
			SCOPE_TIMER("MATERIAL_ARRAY_UPLOAD");
			resetMaterialPtrs();

			std::shared_ptr<ResourceCache> cache = scene->getCache();
			std::vector<std::shared_ptr<Material>> materials = cache->getByType<Material>(ResourceType::Material);


			int m = 0;
			for (auto& material : materials)
			{
				if (!material->GetRefCount() || rendererData.materialMapCache.find(material->GetID()) != rendererData.materialMapCache.end())
					continue;
				rendererData.materialMapCache[material->GetID()] = m++;

				auto diffuseResource = cache->GetByUUID<ImageTexture>(material->getDiffuseID());
				auto normalResource = cache->GetByUUID<ImageTexture>(material->getNormalID());
				auto aoResource = cache->GetByUUID<ImageTexture>(material->getAoID());
				auto roughnessResource = cache->GetByUUID<ImageTexture>(material->getRoughnessID());
				auto metallicResource = cache->GetByUUID<ImageTexture>(material->getMetallicID());

				auto diffuseHandle = diffuseResource->getTextureHandle().handle;
				auto normalHandle = normalResource->getTextureHandle().handle;
				auto aoHandle = aoResource->getTextureHandle().handle;
				auto roughnessHandle = roughnessResource->getTextureHandle().handle;
				auto metallicHandle = metallicResource->getTextureHandle().handle;

				int poolIndex = rendererData.materialUploadPtr;

				MaterialUpload* textureMapPtr = (MaterialUpload*)rendererData.materialBufferPtr;
				textureMapPtr[poolIndex] = {
					diffuseHandle,normalHandle,
					aoHandle,roughnessHandle,
					metallicHandle,
					0,
					 material->getColor(),
					 glm::vec4(material->getUVScale(), material->getNormalStrength(), 1.0f)
				};

				rendererData.materialUploadPtr++;

				//Stats
				RendererStats.totalTextureBufferSize += diffuseResource->GetBytSize();
				RendererStats.totalTextureBufferSize += normalResource->GetBytSize();
				RendererStats.totalTextureBufferSize += aoResource->GetBytSize();
				RendererStats.totalTextureBufferSize += roughnessResource->GetBytSize();
				RendererStats.totalTextureBufferSize += metallicResource->GetBytSize();
			}
		}

		void Iaonnis::Renderer3D::UploadLightData(Scene* scene)
		{
			SCOPE_TIMER("LIGHT_UPLOAD");

			for (auto& entt : scene->getEntitiesWith<LightComponent>())
			{
				auto& lightComp = entt.GetComponent<LightComponent>();
				auto& transform = entt.GetComponent<TransformComponent>();
				if (entt.active == false)
					continue;

				switch (lightComp.type)
				{
					case LightType::Directional: {
						DirectionalLightUpload dLight;
						dLight.color = lightComp.color;
						dLight.direction = glm::vec4(glm::normalize(transform.model * glm::vec4(lightComp.position, 0.0f)));

						rendererData.directionalLightArr[rendererData.lightMeta.nDirectionLight++] = dLight;
						break;
					}
					case LightType::Spot:
					{
						SpotLightUpload sLight;
						sLight.color = lightComp.color;
						sLight.color.w = glm::cos(glm::radians(lightComp.outerRadius));

						sLight.position = transform.model * glm::vec4(lightComp.position, 1.0);
						sLight.position.w = glm::cos(glm::radians(lightComp.innerRadius));
						
						sLight.direction = transform.model * glm::vec4(lightComp.spotDirection, 1.0f);

						rendererData.spotLightArr[rendererData.lightMeta.nSpotLight++] = sLight;
						break;
					}
					case LightType::Point:
					{
						PointLightUpload pLight;
						pLight.color = lightComp.color;
						pLight.position = transform.model * glm::vec4(lightComp.position,1.0f);

						rendererData.pointLightArr[rendererData.lightMeta.nPointLight++] = pLight;
						break;
					}
				}
			}
			
			rendererData.lightMeta.viewPos = glm::vec4(scene->GetSceneCamera()->getFrustrum().position, 1.0f);

			{
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.directionalLightSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(DirectionalLightUpload) * rendererData.lightMeta.nDirectionLight, &rendererData.directionalLightArr[0]);

				glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.spotLigtSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(SpotLightUpload) * rendererData.lightMeta.nSpotLight, &rendererData.spotLightArr[0]);

				glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.pointLigtSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(PointLightUpload) * rendererData.lightMeta.nPointLight, &rendererData.pointLightArr[0]);

				glBindBuffer(GL_UNIFORM_BUFFER, rendererData.lightMetaUBO);
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightMeta), &rendererData.lightMeta);
			}
		}

		void GBufferPass(Scene* scene)
		{
			IGPUResource::bindFramebuffer(rendererData.gBuffer);
			drawCommands(scene, rendererData.gbufferProgram);
		}

		void Iaonnis::Renderer3D::LightPass(Scene* scene)
		{
			resetLightPtrs();
			IGPUResource::bindFramebuffer(rendererData.lightPassFBO);

			glClearColor(0.40f, 0.40f, 0.40f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindVertexArray(rendererData.screenQuadVao);

			glUseProgram(rendererData.lightProgram);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, rendererData.gBuffer.m_Handles[(int)gBufferHandles::Albedo].m_ID);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, rendererData.gBuffer.m_Handles[(int)gBufferHandles::Position].m_ID);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, rendererData.gBuffer.m_Handles[(int)gBufferHandles::Normal].m_ID);

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, rendererData.gBuffer.m_Handles[(int)gBufferHandles::AO].m_ID);

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, rendererData.gBuffer.m_Handles[(int)gBufferHandles::Roughness].m_ID);

			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, rendererData.gBuffer.m_Handles[(int)gBufferHandles::Metallic].m_ID);

			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, rendererData.depthTexture);


			int location = glGetUniformLocation(rendererData.lightProgram, "albedo");
			glUniform1i(location, 0);
			location = glGetUniformLocation(rendererData.lightProgram, "position");
			glUniform1i(location, 1);
			location = glGetUniformLocation(rendererData.lightProgram, "normal");
			glUniform1i(location, 2);
			location = glGetUniformLocation(rendererData.lightProgram, "iAo");
			glUniform1i(location, 3);
			location = glGetUniformLocation(rendererData.lightProgram, "iRoughness");
			glUniform1i(location, 4);
			location = glGetUniformLocation(rendererData.lightProgram, "iMetallic");
			glUniform1i(location, 5);
			location = glGetUniformLocation(rendererData.lightProgram, "iDepthMap");
			glUniform1i(location, 6);

			location = glGetUniformLocation(rendererData.lightProgram, "lightMatrix");
			glm::mat4 viewProj = rendererData.depthProjection * rendererData.depthView;
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(viewProj));
			
			/*location = glGetUniformLocation(rendererData.lightProgram, "viewMat");
			auto view = scene->GetSceneCamera()->getView();
			glUniformMatrix4fv(location, 1, GL_FALSE, &view[0][0]);*/

			UploadLightData(scene);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}


		void SubmitDrawCommandData(DrawData data)
		{
			//Transform Index Space: To be continious of the already batch vertices
			uint32_t* indexPtr = static_cast<uint32_t*>(rendererData.eboPtr);

			uint32_t firstIndex = *(uint32_t*)data.indexPtr;
			uint32_t* srcIndex = (uint32_t*)data.indexPtr;
			for (int i = 0; i < data.indexCount; i++)
			{
				uint32_t in = srcIndex[i];
				uint32_t generatedIndex = in + rendererData.currentVertexCount - firstIndex;
				indexPtr[(rendererData.indexCount + rendererData.currentIndexCount) + i] = generatedIndex;

			}

			Vertice* vboPtr = (Vertice*)rendererData.vboPtr;
			Vertice* verticePtr = (Vertice*)data.vertexPtr;
			for (int i = 0; i < data.vertexCount; i++)
			{
				vboPtr[(rendererData.vertexCount + rendererData.currentVertexCount) + i] = verticePtr[i];
			}

			CommandData* cmdDataPtr = (CommandData*)rendererData.commandDataBufferPtr;
			cmdDataPtr[rendererData.commnadDataBufferOffset].nMeshes++;

			rendererData.currentVertexCount += data.vertexCount;
			rendererData.currentIndexCount += data.indexCount;

			RendererStats.nRenderedVertices += data.vertexCount;
			RendererStats.nRenderedIndices += data.indexCount;			
		}

		void CloseDrawCommands()
		{
			SCOPE_TIMER(__FUNCTION__);

			DrawElementsIndirectCommand cm;
			cm.baseInstance = 0;
			cm.baseVertex = rendererData.vertexCount;
			cm.count = rendererData.currentIndexCount;
			cm.firstIndex = rendererData.indexCount;
			cm.instanceCount = 1;

			rendererData.vertexCount += rendererData.currentVertexCount;
			rendererData.indexCount += rendererData.currentIndexCount;

			rendererData.currentIndexCount = 0;
			rendererData.currentVertexCount = 0;

			DrawElementsIndirectCommand* iboPtr = (DrawElementsIndirectCommand*)rendererData.iboPtr;
			iboPtr[rendererData.commandPtr] = cm;

			CommandData* cmdDataPtr = (CommandData*)rendererData.commandDataBufferPtr;
			cmdDataPtr[rendererData.commnadDataBufferOffset].offset = rendererData.subMeshOffset;

			rendererData.subMeshOffset += cmdDataPtr[rendererData.commnadDataBufferOffset].nMeshes;

			rendererData.commnadDataBufferOffset++;

			rendererData.commandPtr++;
		
			RendererStats.nDrawCalls++;

		}

		void drawCommands(Scene* scene, uint32_t program)
		{
			SCOPE_TIMER(__FUNCTION__);

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(program);

			 glm::mat4 mvp = scene->GetSceneCamera()->getViewProject();

			glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, &mvp[0][0]);

			glBindVertexArray(rendererData.vao);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, rendererData.ibo);
			glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, rendererData.commandPtr, 0);

		}

		void Iaonnis::Renderer3D::resetGeometryPtrs()
		{
			SCOPE_TIMER(__FUNCTION__);

			rendererData.commandPtr = 0;
			rendererData.currentVertexCount = 0;
			rendererData.currentIndexCount = 0;
			rendererData.indexCount = 0;
			rendererData.vertexCount = 0; 

			rendererData.subMeshOffset = 0;
			rendererData.commnadDataBufferOffset = 0;

			RendererStats.nDrawCalls = 0;
			RendererStats.nRenderedIndices = 0;
			RendererStats.nRenderedVertices = 0;
			RendererStats.totalTextureBufferSize = 0;
		}

		void resetLightPtrs()
		{
			rendererData.lightMeta.nDirectionLight = 0;
			rendererData.lightMeta.nSpotLight = 0;
			rendererData.lightMeta.nPointLight = 0;
		}

		void resetMaterialPtrs()
		{
			rendererData.materialUploadPtr = 0; 
			rendererData.materialMapCache.clear();
		}

		void RenderScene(Scene* scene, uint32_t program)
		{
			SCOPE_TIMER(__FUNCTION__);

			if (!scene)
			{
				return;
			}

			if (scene->IsEntityRegisteryDirty())
			{
				WaitFence(rendererData.gSync);

				UploadScene(scene);
				scene->SetEntityRegisteryClean();
			}

			if (scene->IsMaterialsDirty())
			{
				WaitFence(rendererData.gSync);

				UploadMaterialArray(scene);
				scene->SetMaterialClean();
			}
			UploadLightData(scene);

			RenderShadowMap(scene);
			
			GBufferPass(scene);

			LockFence(rendererData.gSync);

			LightPass(scene);
			//EnvironmentPass(scene);

		}

		uint32_t GetRenderOutput()
		{
			//return rendererData.depthTexture;
			return rendererData.lightPassFBO.m_Handles[0].m_ID;
		}

		void OnViewFrameResize(Event& event)
		{
			SCOPE_TIMER(__FUNCTION__);

			FrameResizeEvent* frameResizeEvent = (FrameResizeEvent*)&event;
			rendererData.frameSize = glm::vec2(frameResizeEvent->frameSizeX, frameResizeEvent->frameSizeY);

			glViewport(0, 0, frameResizeEvent->frameSizeX, frameResizeEvent->frameSizeY);

			FRAMEBUFFER_DESC fboDesc;
			fboDesc.n_Desc = 3;
			fboDesc.textureDesc = (TEXTURE_DESC*)malloc(sizeof(TEXTURE_DESC) * fboDesc.n_Desc);
			fboDesc.textureDesc[0].dataType = TEXTURE_DATA::TEXTURE_COLOR;
			fboDesc.textureDesc[0].height = frameResizeEvent->frameSizeY;
			fboDesc.textureDesc[0].width = frameResizeEvent->frameSizeX;
			fboDesc.textureDesc[0].nBitPerChannel = 32;
			fboDesc.textureDesc[0].nChannels = 4;
			fboDesc.textureDesc[0].ptr = nullptr;
			fboDesc.textureDesc[0].x = 0;
			fboDesc.textureDesc[0].y = 0;

			fboDesc.textureDesc[1].dataType = TEXTURE_DATA::TEXTURE_COLOR;
			fboDesc.textureDesc[1].height = frameResizeEvent->frameSizeY;
			fboDesc.textureDesc[1].width = frameResizeEvent->frameSizeX;
			fboDesc.textureDesc[1].nBitPerChannel = 32;
			fboDesc.textureDesc[1].nChannels = 4;
			fboDesc.textureDesc[1].ptr = nullptr;
			fboDesc.textureDesc[1].x = 0;
			fboDesc.textureDesc[1].y = 0;

			fboDesc.textureDesc[2].dataType = TEXTURE_DATA::TEXTURE_DEPTH;
			fboDesc.textureDesc[2].height = frameResizeEvent->frameSizeY;
			fboDesc.textureDesc[2].width = frameResizeEvent->frameSizeX;
			fboDesc.textureDesc[2].nBitPerChannel = 24;
			fboDesc.textureDesc[2].nChannels = 1;
			fboDesc.textureDesc[2].ptr = nullptr;
			fboDesc.textureDesc[2].x = 0;
			fboDesc.textureDesc[2].y = 0;

			rendererData.lightPassFBO = IGPUResource::resizeFramebuffer(fboDesc, rendererData.lightPassFBO);
			free(fboDesc.textureDesc);

			FRAMEBUFFER_DESC gBufferDesc;
			gBufferDesc.n_Desc = 7;
			gBufferDesc.textureDesc = (TEXTURE_DESC*)malloc(sizeof(TEXTURE_DESC) * gBufferDesc.n_Desc);
			gBufferDesc.textureDesc[(int)gBufferHandles::Albedo].dataType = TEXTURE_DATA::TEXTURE_COLOR;
			gBufferDesc.textureDesc[(int)gBufferHandles::Albedo].height = frameResizeEvent->frameSizeY;
			gBufferDesc.textureDesc[(int)gBufferHandles::Albedo].width = frameResizeEvent->frameSizeX;
			gBufferDesc.textureDesc[(int)gBufferHandles::Albedo].nBitPerChannel = 32;
			gBufferDesc.textureDesc[(int)gBufferHandles::Albedo].nChannels = 4;
			gBufferDesc.textureDesc[(int)gBufferHandles::Albedo].ptr = nullptr;
			gBufferDesc.textureDesc[(int)gBufferHandles::Albedo].x = 0;
			gBufferDesc.textureDesc[(int)gBufferHandles::Albedo].y = 0;

			gBufferDesc.textureDesc[(int)gBufferHandles::Position].dataType = TEXTURE_DATA::TEXTURE_COLOR;
			gBufferDesc.textureDesc[(int)gBufferHandles::Position].height = frameResizeEvent->frameSizeY;
			gBufferDesc.textureDesc[(int)gBufferHandles::Position].width = frameResizeEvent->frameSizeX;
			gBufferDesc.textureDesc[(int)gBufferHandles::Position].nBitPerChannel = 32;
			gBufferDesc.textureDesc[(int)gBufferHandles::Position].nChannels = 4;
			gBufferDesc.textureDesc[(int)gBufferHandles::Position].ptr = nullptr;
			gBufferDesc.textureDesc[(int)gBufferHandles::Position].x = 0;
			gBufferDesc.textureDesc[(int)gBufferHandles::Position].y = 0;

			gBufferDesc.textureDesc[(int)gBufferHandles::Normal].dataType = TEXTURE_DATA::TEXTURE_COLOR;
			gBufferDesc.textureDesc[(int)gBufferHandles::Normal].height = frameResizeEvent->frameSizeY;
			gBufferDesc.textureDesc[(int)gBufferHandles::Normal].width = frameResizeEvent->frameSizeX;
			gBufferDesc.textureDesc[(int)gBufferHandles::Normal].nBitPerChannel = 32;
			gBufferDesc.textureDesc[(int)gBufferHandles::Normal].nChannels = 4;
			gBufferDesc.textureDesc[(int)gBufferHandles::Normal].ptr = nullptr;
			gBufferDesc.textureDesc[(int)gBufferHandles::Normal].x = 0;
			gBufferDesc.textureDesc[(int)gBufferHandles::Normal].y = 0;

			gBufferDesc.textureDesc[(int)gBufferHandles::AO].dataType = TEXTURE_DATA::TEXTURE_COLOR;
			gBufferDesc.textureDesc[(int)gBufferHandles::AO].height = frameResizeEvent->frameSizeY;
			gBufferDesc.textureDesc[(int)gBufferHandles::AO].width = frameResizeEvent->frameSizeX;
			gBufferDesc.textureDesc[(int)gBufferHandles::AO].nBitPerChannel = 32;
			gBufferDesc.textureDesc[(int)gBufferHandles::AO].nChannels = 1;
			gBufferDesc.textureDesc[(int)gBufferHandles::AO].ptr = nullptr;
			gBufferDesc.textureDesc[(int)gBufferHandles::AO].x = 0;
			gBufferDesc.textureDesc[(int)gBufferHandles::AO].y = 0;

			gBufferDesc.textureDesc[(int)gBufferHandles::Roughness].dataType = TEXTURE_DATA::TEXTURE_COLOR;
			gBufferDesc.textureDesc[(int)gBufferHandles::Roughness].height = frameResizeEvent->frameSizeY;
			gBufferDesc.textureDesc[(int)gBufferHandles::Roughness].width = frameResizeEvent->frameSizeX;
			gBufferDesc.textureDesc[(int)gBufferHandles::Roughness].nBitPerChannel = 32;
			gBufferDesc.textureDesc[(int)gBufferHandles::Roughness].nChannels = 1;
			gBufferDesc.textureDesc[(int)gBufferHandles::Roughness].ptr = nullptr;
			gBufferDesc.textureDesc[(int)gBufferHandles::Roughness].x = 0;
			gBufferDesc.textureDesc[(int)gBufferHandles::Roughness].y = 0;

			gBufferDesc.textureDesc[(int)gBufferHandles::Metallic].dataType = TEXTURE_DATA::TEXTURE_COLOR;
			gBufferDesc.textureDesc[(int)gBufferHandles::Metallic].height = frameResizeEvent->frameSizeY;
			gBufferDesc.textureDesc[(int)gBufferHandles::Metallic].width = frameResizeEvent->frameSizeX;
			gBufferDesc.textureDesc[(int)gBufferHandles::Metallic].nBitPerChannel = 32;
			gBufferDesc.textureDesc[(int)gBufferHandles::Metallic].nChannels = 1;
			gBufferDesc.textureDesc[(int)gBufferHandles::Metallic].ptr = nullptr;
			gBufferDesc.textureDesc[(int)gBufferHandles::Metallic].x = 0;
			gBufferDesc.textureDesc[(int)gBufferHandles::Metallic].y = 0;

			gBufferDesc.textureDesc[(int)gBufferHandles::Depth].dataType = TEXTURE_DATA::TEXTURE_DEPTH;
			gBufferDesc.textureDesc[(int)gBufferHandles::Depth].height = frameResizeEvent->frameSizeY;
			gBufferDesc.textureDesc[(int)gBufferHandles::Depth].width = frameResizeEvent->frameSizeX;
			gBufferDesc.textureDesc[(int)gBufferHandles::Depth].nBitPerChannel = 24;
			gBufferDesc.textureDesc[(int)gBufferHandles::Depth].nChannels = 1;
			gBufferDesc.textureDesc[(int)gBufferHandles::Depth].ptr = nullptr;
			gBufferDesc.textureDesc[(int)gBufferHandles::Depth].x = 0;
			gBufferDesc.textureDesc[(int)gBufferHandles::Depth].y = 0;

			rendererData.gBuffer = IGPUResource::resizeFramebuffer(gBufferDesc, rendererData.gBuffer);
			free(gBufferDesc.textureDesc);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		}

		RendererStatistics GetRenderStats()
		{
			return RendererStats;
		}
	}
}
