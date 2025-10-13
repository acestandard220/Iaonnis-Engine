#include "Renderer.h"

namespace Iaonnis {
	namespace Renderer3D {

		GLuint CreateShaderProgram(const char* vertexPath, const char* fragmentPath)
		{
			// Helper to read file contents
			auto readFile = [](const char* path) -> std::string {
				std::ifstream file(path, std::ios::in | std::ios::binary);
				if (!file) return "";
				std::string contents;
				file.seekg(0, std::ios::end);
				contents.resize(static_cast<size_t>(file.tellg()));
				file.seekg(0, std::ios::beg);
				file.read(&contents[0], contents.size());
				file.close();
				return contents;
				};

			std::string vertexCode = readFile(vertexPath);
			std::string fragmentCode = readFile(fragmentPath);

			if (vertexCode.empty() || fragmentCode.empty())
				return 0;

			auto checkShader = [](GLuint shader, const char* type) {
				GLint success;
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if (!success) {
					GLint logLength = 0;
					glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
					std::string infoLog(logLength, ' ');
					glGetShaderInfoLog(shader, logLength, nullptr, &infoLog[0]);
					fprintf(stderr, "%s shader compilation failed:\n%s\n", type, infoLog.c_str());
				}
				};

			auto checkProgram = [](GLuint program) {
				GLint success;
				glGetProgramiv(program, GL_LINK_STATUS, &success);
				if (!success) {
					GLint logLength = 0;
					glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
					std::string infoLog(logLength, ' ');
					glGetProgramInfoLog(program, logLength, nullptr, &infoLog[0]);
					fprintf(stderr, "Program linking failed:\n%s\n", infoLog.c_str());
				}
				};

			// Compile vertex shader
			GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
			const char* vSource = vertexCode.c_str();
			glShaderSource(vertexShader, 1, &vSource, nullptr);
			glCompileShader(vertexShader);
			checkShader(vertexShader, "Vertex");

			// Compile fragment shader
			GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			const char* fSource = fragmentCode.c_str();
			glShaderSource(fragmentShader, 1, &fSource, nullptr);
			glCompileShader(fragmentShader);
			checkShader(fragmentShader, "Fragment");

			// Create shader program
			GLuint shaderProgram = glCreateProgram();
			glAttachShader(shaderProgram, vertexShader);
			glAttachShader(shaderProgram, fragmentShader);
			glLinkProgram(shaderProgram);
			checkProgram(shaderProgram);

			// Cleanup shaders
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			// Check for link errors and return 0 if failed
			GLint linkStatus = GL_FALSE;
			glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
			if (linkStatus != GL_TRUE) {
				glDeleteProgram(shaderProgram);
				return 0;
			}

			return shaderProgram;
		}

		struct MaterialUpload
		{
			glm::vec4 color;
			glm::vec4 uvScale; //+ normal strength + flipY
		};

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

		struct {
			uint32_t vao;
			uint32_t vbo;
			uint32_t ebo;
			uint32_t ibo;

			uint32_t screenQuadVao;
			uint32_t screenQuadVbo;
			uint32_t screenQuadEbo;

			uint32_t environmentCubeVao;
			uint32_t environmentCubeVbo;
			uint32_t environmentCubeEbo;

			uint32_t lightProgram;
			uint32_t environmentProgram;

			uint32_t modelSSBO;
			//uint32_t cameraUBO;

			uint32_t diffuseSSBO;
			uint32_t normalSSBO;
			uint32_t aoSSBO;
			uint32_t roughnessSSBO;
			uint32_t metallicSSBO;

			uint32_t materialUBO;

			uint32_t directionalLightSSBO;
			uint32_t spotLigtSSBO;
			uint32_t pointLigtSSBO;
			uint32_t lightMetaUBO;

			FramebufferHandle fbo;
			FramebufferHandle gBuffer;

			static const int MAX_VERTEX = 300000;
			static const int MAX_INDICES = 500000;

			const int MAX_DRAW_COMMANDS = 10;
			int commandPtr = 0;

			int indexCount = 0;
			int vertexCount = 0;
			int currentVertexCount = 0;
			int currentIndexCount = 0;

			static const int MAX_MATERIALS = 100;
			GLuint64 diffuseUploadArr[MAX_MATERIALS];
			GLuint64 normalUploadArr[MAX_MATERIALS];
			GLuint64 aoUploadArr[MAX_MATERIALS];
			GLuint64 roughnessUploadArr[MAX_MATERIALS];
			GLuint64 metallicUploadArr[MAX_MATERIALS];

			MaterialUpload materialUploadArr[MAX_MATERIALS];

			static const int MAX_TYPE_OF_LIGHT = 100;
			DirectionalLightUpload directionalLightArr[MAX_TYPE_OF_LIGHT];
			SpotLightUpload spotLightArr[MAX_TYPE_OF_LIGHT];
			PointLightUpload pointLightArr[MAX_TYPE_OF_LIGHT];

			LightMeta lightMeta{};

			int materialUploadPtr = 0;
		}rendererData;

		RendererStatistics RendererStats{};


		void CreateLightBuffers()
		{
			glGenBuffers(1, &rendererData.directionalLightSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.directionalLightSSBO);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(DirectionalLightUpload) * rendererData.MAX_TYPE_OF_LIGHT, nullptr, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_SLOT::DirectionalLight, rendererData.directionalLightSSBO);

			glGenBuffers(1, &rendererData.spotLigtSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.spotLigtSSBO);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(SpotLightUpload) * rendererData.MAX_TYPE_OF_LIGHT, nullptr, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_SLOT::SpotLight, rendererData.spotLigtSSBO);

			glGenBuffers(1, &rendererData.pointLigtSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.pointLigtSSBO);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(PointLightUpload) * rendererData.MAX_TYPE_OF_LIGHT, nullptr, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_SLOT::PointLight, rendererData.pointLigtSSBO);

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

			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertice) * rendererData.MAX_VERTEX, nullptr, GL_DYNAMIC_DRAW);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * rendererData.MAX_INDICES, nullptr, GL_DYNAMIC_DRAW);
			glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand) * rendererData.MAX_DRAW_COMMANDS, nullptr, GL_DYNAMIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), 0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, n));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, uv));
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, tangent));
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, bitangent));

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
			glEnableVertexAttribArray(4);

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

		void Iaonnis::Renderer3D::Initialize(uint32_t program)
		{
			//====================Shader Creation================================================

			rendererData.lightProgram = CreateShaderProgram("Assets/Shaders/lightVert.glsl", "Assets/Shaders/pbrFragment.glsl");
			rendererData.environmentProgram = CreateShaderProgram("Assets/Shaders/environmentVert.glsl", "Assets/Shaders/environmentFrag.glsl");
			//===================================================================================

			CreateIndirectDrawBuffers();

			//=======================Texture SSBO=================================
			glGenBuffers(1, &rendererData.diffuseSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.diffuseSSBO);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint64) * rendererData.MAX_MATERIALS, nullptr, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_SLOT::Diffuse, rendererData.diffuseSSBO);

			glGenBuffers(1, &rendererData.normalSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.normalSSBO);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint64) * rendererData.MAX_MATERIALS, nullptr, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_SLOT::Normal, rendererData.normalSSBO);

			glGenBuffers(1, &rendererData.aoSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.aoSSBO);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint64) * rendererData.MAX_MATERIALS, nullptr, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_SLOT::AO, rendererData.aoSSBO);

			glGenBuffers(1, &rendererData.roughnessSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.roughnessSSBO);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint64) * rendererData.MAX_MATERIALS, nullptr, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_SLOT::Roughness, rendererData.roughnessSSBO);

			glGenBuffers(1, &rendererData.metallicSSBO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.metallicSSBO);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint64) * rendererData.MAX_MATERIALS, nullptr, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_SLOT::Metallic, rendererData.metallicSSBO);

			CreateLightBuffers();

			//====================Material UBO======================================
			glGenBuffers(1, &rendererData.materialUBO);
			glBindBuffer(GL_UNIFORM_BUFFER, rendererData.materialUBO);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialUpload) * rendererData.MAX_MATERIALS, nullptr, GL_DYNAMIC_DRAW);
			glBindBufferRange(GL_UNIFORM_BUFFER, 0, rendererData.materialUBO, 0, sizeof(MaterialUpload) * rendererData.MAX_MATERIALS);
			glUniformBlockBinding(program, glGetUniformBlockIndex(program, "Materials"), 0);


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

			//====================Environment Cube Buffers=========================

			float environmentCubeVertices[]
			{
				-1.0f,-1.0f,-1.0f,
				 1.0f,-1.0f,-1.0f,
				 1.0f, 1.0f,-1.0f,
				-1.0f, 1.0f,-1.0f,

				 1.0f,-1.0f, 1.0f,
				-1.0f,-1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f,
				 1.0f, 1.0f, 1.0f,

				-1.0f,-1.0f, 1.0f,
				-1.0f,-1.0f,-1.0f,
				-1.0f, 1.0f,-1.0f,
				-1.0f, 1.0f, 1.0f,

				1.0f,-1.0f,-1.0f,
				1.0f,-1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f,-1.0f,

				-1.0f, 1.0f,-1.0f,
				 1.0f, 1.0f,-1.0f,
				 1.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f,

				-1.0f,-1.0f, 1.0f,
				 1.0f,-1.0f, 1.0f,
				 1.0f,-1.0f,-1.0f,
				-1.0f,-1.0f,-1.0f
			};

			float environmentCubeIndices[] =
			{
				0,1,2,
				2,3,0,

				4,5,6,
				6,7,4,

				8,9,10,
				10,11,8,

				12,13,14,
				14,15,12,

				16,17,18,
				18,19,16,

				20,21,22,
				22,23,20,
			};


			glGenVertexArrays(1, &rendererData.environmentCubeVao);
			glBindVertexArray(rendererData.environmentCubeVao);

			glGenBuffers(1, &rendererData.environmentCubeVbo);
			glBindBuffer(GL_ARRAY_BUFFER, rendererData.environmentCubeVbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 72, environmentCubeVertices, GL_STATIC_DRAW);

			glGenBuffers(1, &rendererData.environmentCubeEbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererData.environmentCubeEbo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 36, environmentCubeIndices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

			glEnableVertexAttribArray(0);

			glBindVertexArray(0);

			//=====================================================================

			//====================FBO================================
			FRAMEBUFFER_DESC fboDesc;
			fboDesc.n_Desc = 2;
			fboDesc.textureDesc = (TEXTURE_DESC*)malloc(sizeof(TEXTURE_DESC) * fboDesc.n_Desc);
			fboDesc.textureDesc[0].dataType = TEXTURE_DATA::TEXTURE_COLOR;
			fboDesc.textureDesc[0].height = 800;
			fboDesc.textureDesc[0].width = 800;
			fboDesc.textureDesc[0].nBitPerChannel = 32;
			fboDesc.textureDesc[0].nChannels = 4;
			fboDesc.textureDesc[0].ptr = nullptr;
			fboDesc.textureDesc[0].x = 0;
			fboDesc.textureDesc[0].y = 0;

			fboDesc.textureDesc[1].dataType = TEXTURE_DATA::TEXTURE_DEPTH;
			fboDesc.textureDesc[1].height = 800;
			fboDesc.textureDesc[1].width = 800;
			fboDesc.textureDesc[1].nBitPerChannel = 24;
			fboDesc.textureDesc[1].nChannels = 1;
			fboDesc.textureDesc[1].ptr = nullptr;
			fboDesc.textureDesc[1].x = 0;
			fboDesc.textureDesc[1].y = 0;

			rendererData.fbo = IGPUResource::createFrambuffer(fboDesc, FBO_FLAGS::ALL_COLOR_ATTACHMENT);
			free(fboDesc.textureDesc);

			CreateGBuffer();

			//=====================================
			rendererData.commandPtr = 0;
			rendererData.indexCount = 0;
			rendererData.vertexCount = 0;
			rendererData.currentIndexCount = 0;
			//=====================================

			EventBus::subscribe(EventType::RESIZE_EVENT, OnViewFrameResize);
		}

		void Shutdown()
		{
			glDeleteVertexArrays(1, &rendererData.vao);
			glDeleteBuffers(1, &rendererData.vbo);
			glDeleteBuffers(1, &rendererData.ebo);
			glDeleteBuffers(1, &rendererData.ibo);

			glDeleteVertexArrays(1, &rendererData.screenQuadVao);
			glDeleteBuffers(1, &rendererData.screenQuadVbo);
			glDeleteBuffers(1, &rendererData.screenQuadEbo);

			glDeleteVertexArrays(1, &rendererData.environmentCubeVao);
			glDeleteBuffers(1, &rendererData.environmentCubeVbo);
			glDeleteBuffers(1, &rendererData.environmentCubeEbo);

			glDeleteProgram(rendererData.lightProgram);
			glDeleteProgram(rendererData.environmentProgram);

			glDeleteBuffers(1, &rendererData.diffuseSSBO);
			glDeleteBuffers(1, &rendererData.normalSSBO);

			glDeleteBuffers(1, &rendererData.materialUBO);

			glDeleteBuffers(1, &rendererData.directionalLightSSBO);
			glDeleteBuffers(1, &rendererData.spotLigtSSBO);
			glDeleteBuffers(1, &rendererData.pointLigtSSBO);

			glDeleteBuffers(1, &rendererData.lightMetaUBO);


			IGPUResource::DeleteFramebuffer(rendererData.gBuffer);
			IGPUResource::DeleteFramebuffer(rendererData.fbo);

			IAONNIS_LOG_INFO("Renderer has shutdown");
		}

		void Iaonnis::Renderer3D::EnvironmentPass(Scene* scene)
		{
			// Make global...
			IGPUResource::bindFramebuffer(rendererData.fbo);
			glDepthMask(GL_FALSE);
			glUseProgram(rendererData.environmentProgram);

			glBindVertexArray(rendererData.environmentCubeVao);

			glm::mat4 ivp = (scene->GetSceneCamera()->getViewProject());
			int location = glGetUniformLocation(rendererData.environmentProgram, "ivp");

			glUniformMatrix4fv(location, 1, GL_FALSE, &ivp[0][0]);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, scene->GetEnvironment()->GetCubeMapHandle().m_ID);

			location = glGetUniformLocation(rendererData.environmentProgram, "environmentMap");
			if (location == -1)
			{
				IAONNIS_LOG_ERROR("FAiled to find location in shader\.");
			}
			glUniform1i(location, 0);

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

			glDepthMask(GL_TRUE);
		}

		void Iaonnis::Renderer3D::UploadScene(Scene* scene)
		{
			CPUTimer timer;
			timer.start();

			resetGeometryPtrs();
			std::shared_ptr<ResourceCache> cache = scene->getCache();
			std::vector<std::shared_ptr<Material>> materials = cache->getByType<Material>(ResourceType::Material);

			for (auto& material : materials)
			{
				UUID materialID = material->GetID();
				bool inUse = false;
				for (auto& entt : scene->getEntitiesWith<MeshFilterComponent>())
				{
					if (!entt.active)
					{
						IAONNIS_LOG_INFO("Skipping render of inactive entity.");
						continue;
					}
					auto meshFilter = entt.GetComponent<MeshFilterComponent>();
					auto meshTransformMatrix = entt.GetTransformMatrix();


					auto hasMaterial = meshFilter.materialIDMap.find(materialID);
					if (hasMaterial == meshFilter.materialIDMap.end())
					{
						continue;
					}
					auto materialDependants = meshFilter.materialIDMap[materialID];

					for (auto index : materialDependants)
					{
						std::shared_ptr<Mesh> meshResource = cache->getByUUID<Mesh>(meshFilter.meshID);
						if (!meshResource)
							continue;

						SubMesh* subMesh = meshResource->getSubMesh(index);
						Vertice* subMeshVertices = meshResource->getSubMeshVerticeStart(index);

						std::vector<Vertice> transformedVertices(subMesh->vertexCount);
						for (int v = 0; v < subMesh->vertexCount; v++)
						{
							transformedVertices[v] = subMeshVertices[v];
							auto transformedVec4 = (meshTransformMatrix * glm::vec4(transformedVertices[v].p, 1.0F));
							transformedVertices[v].n = glm::normalize(glm::mat3(glm::transpose(glm::inverse(meshTransformMatrix))) * transformedVertices[v].n);
							transformedVertices[v].tangent = glm::normalize(glm::mat3(glm::transpose(glm::inverse(meshTransformMatrix))) * transformedVertices[v].tangent);

							transformedVertices[v].p = transformedVec4;
						}

						DrawData data;
						data.indexCount = subMesh->indexCount;
						data.indexPtr = meshResource->getSubMeshIndexStart(index);

						data.vertexCount = subMesh->vertexCount;
						data.vertexPtr = transformedVertices.data();

						inUse = true;
						submitDrawCommandData(data);
					}
				}

				if (inUse)
				{
					closeDrawCommands();
					auto diffuseResource = cache->getByUUID<ImageTexture>(material->getDiffuseID());
					auto normalResource = cache->getByUUID<ImageTexture>(material->getNormalID());
					auto aoResource = cache->getByUUID<ImageTexture>(material->getAoID());
					auto roughnessResource = cache->getByUUID<ImageTexture>(material->getRoughnessID());
					auto metallicResource = cache->getByUUID<ImageTexture>(material->getMetallicID());

					auto diffuseHandle = diffuseResource->getTextureHandle().handle;
					auto normalHandle = normalResource->getTextureHandle().handle;
					auto aoHandle = aoResource->getTextureHandle().handle;
					auto roughnessHandle = roughnessResource->getTextureHandle().handle;
					auto metallicHandle = metallicResource->getTextureHandle().handle;

					MaterialUpload mtlUpload;
					mtlUpload.color = material->getColor();
					mtlUpload.uvScale = glm::vec4(material->getUVScale(), material->getNormalStrength(), 1.0f);

					int poolIndex = rendererData.materialUploadPtr;
					rendererData.diffuseUploadArr[poolIndex] = diffuseHandle;
					rendererData.normalUploadArr[poolIndex] = normalHandle;
					rendererData.aoUploadArr[poolIndex] = aoHandle;
					rendererData.roughnessUploadArr[poolIndex] = roughnessHandle;
					rendererData.metallicUploadArr[poolIndex] = metallicHandle;

					rendererData.materialUploadArr[rendererData.materialUploadPtr] = mtlUpload;
					rendererData.materialUploadPtr++;


					//Stats
					RendererStats.totalTextureBufferSize += diffuseResource->GetBytSize();
					RendererStats.totalTextureBufferSize += normalResource->GetBytSize();
					RendererStats.totalTextureBufferSize += aoResource->GetBytSize();
					RendererStats.totalTextureBufferSize += roughnessResource->GetBytSize();
					RendererStats.totalTextureBufferSize += metallicResource->GetBytSize();
				}

			}
			UploadMaterialsToGPU();

			timer.stop();
			RendererStats.sceneUploadTime = timer.durationMs();
		}

		void UploadSceneNoBatching(Scene* scene)
		{
			resetGeometryPtrs();
			std::shared_ptr<ResourceCache> cache = scene->getCache();
			std::vector<std::shared_ptr<Material>> materials = cache->getByType<Material>(ResourceType::Material);

			for (auto& entity : scene->GetEntities())
			{
				if (!entity.HasComponent<MeshFilterComponent>())
					continue;

				auto meshFilter = entity.GetComponent<MeshFilterComponent>();
				auto meshTransformMatrix = entity.GetTransformMatrix();

				std::shared_ptr<Mesh> meshResource = cache->getByUUID<Mesh>(meshFilter.meshID);

				for(auto& [mtlID, mtlDeps]: meshFilter.materialIDMap)
				{
					for (auto& i : mtlDeps)
					{
						std::shared_ptr<Material> material = cache->getByUUID<Material>(mtlID);
						if (!meshResource)
							continue;

						SubMesh* subMesh = meshResource->getSubMesh(i);
						Vertice* subMeshVertices = meshResource->getSubMeshVerticeStart(i);

						DrawData data;
						data.indexCount = subMesh->indexCount;
						data.indexPtr = meshResource->getSubMeshIndexStart(i);

						data.vertexCount = subMesh->vertexCount;
						data.vertexPtr = subMeshVertices;

						submitDrawCommandData(data);
						closeDrawCommands();

						auto diffuseResource = cache->getByUUID<ImageTexture>(material->getDiffuseID());
						auto normalResource = cache->getByUUID<ImageTexture>(material->getNormalID());
						auto diffuseHandle = diffuseResource->getTextureHandle().handle;
						auto normalHandle = normalResource->getTextureHandle().handle;

						MaterialUpload mtlUpload;
						mtlUpload.color = material->getColor();
						mtlUpload.uvScale = glm::vec4(material->getUVScale(), material->getNormalStrength(), 1.0f);

						rendererData.diffuseUploadArr[rendererData.materialUploadPtr] = diffuseHandle;
						rendererData.normalUploadArr[rendererData.materialUploadPtr] = normalHandle;
						rendererData.materialUploadArr[rendererData.materialUploadPtr] = mtlUpload;
						rendererData.materialUploadPtr++;
					}
				}
			}

			UploadMaterialsToGPU();
		}

		void UploadMaterialArray()
		{

		}

		void Iaonnis::Renderer3D::UploadMaterialsToGPU()
		{
			CPUTimer timer;
			timer.start();

			glBindBuffer(GL_UNIFORM_BUFFER, rendererData.materialUBO);
			glBufferSubData(GL_UNIFORM_BUFFER,0,rendererData.materialUploadPtr * sizeof(MaterialUpload),&rendererData.materialUploadArr[0]);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.diffuseSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, rendererData.materialUploadPtr * sizeof(GLuint64), &rendererData.diffuseUploadArr[0]);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.normalSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, rendererData.materialUploadPtr * sizeof(GLuint64), &rendererData.normalUploadArr[0]);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.aoSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, rendererData.materialUploadPtr * sizeof(GLuint64), &rendererData.aoUploadArr[0]);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.roughnessSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, rendererData.materialUploadPtr * sizeof(GLuint64), &rendererData.roughnessUploadArr[0]);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.metallicSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, rendererData.materialUploadPtr * sizeof(GLuint64), &rendererData.metallicUploadArr[0]);

			timer.stop();
			RendererStats.materialUploadTime = timer.durationMs();
		}

		void Iaonnis::Renderer3D::UploadLightData(Scene* scene)
		{
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
						sLight.position = glm::vec4(lightComp.position, glm::cos(glm::radians(lightComp.innerRadius)));
						sLight.direction = glm::vec4(lightComp.spotDirection, 1.0f);

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

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.directionalLightSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(DirectionalLightUpload) * rendererData.lightMeta.nDirectionLight, &rendererData.directionalLightArr[0]);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.spotLigtSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(SpotLightUpload) * rendererData.lightMeta.nSpotLight, &rendererData.spotLightArr[0]);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererData.pointLigtSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(PointLightUpload) * rendererData.lightMeta.nPointLight, &rendererData.pointLightArr[0]);

			glBindBuffer(GL_UNIFORM_BUFFER, rendererData.lightMetaUBO);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightMeta), &rendererData.lightMeta);
		}

		void Iaonnis::Renderer3D::LightPass(Scene* scene)
		{
			resetLightPtrs();
			IGPUResource::bindFramebuffer(rendererData.fbo);

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
			

			UploadLightData(scene);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void submitDrawCommandData(DrawData data)
		{
			//Transform Index Space: To be continious of the already batch vertices
			void* indexPtr = data.indexPtr;

			std::vector<uint32_t> newIndices;
			uint32_t firstIndex = *(uint32_t*)data.indexPtr;
			uint32_t* srcIndex = (uint32_t*)data.indexPtr;

			for (int i = 0; i < data.indexCount; i++)
			{
				uint32_t in = srcIndex[i];
				newIndices.push_back(in + rendererData.currentVertexCount - firstIndex);
			}
			indexPtr = newIndices.data();

			glBindVertexArray(rendererData.vao);

			glBindBuffer(GL_ARRAY_BUFFER, rendererData.vbo);
			glBufferSubData(GL_ARRAY_BUFFER, (rendererData.vertexCount + rendererData.currentVertexCount) * sizeof(Vertice), sizeof(Vertice) * data.vertexCount, data.vertexPtr);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererData.ebo);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (rendererData.indexCount + rendererData.currentIndexCount) * sizeof(uint32_t), sizeof(uint32_t) * data.indexCount, indexPtr);

			rendererData.currentVertexCount += data.vertexCount;
			rendererData.currentIndexCount += data.indexCount;

			
			RendererStats.nRenderedVertices += data.vertexCount;
			RendererStats.nRenderedIndices += data.indexCount;

			
		}

		void closeDrawCommands()
		{
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
			
			glBufferSubData(GL_DRAW_INDIRECT_BUFFER, rendererData.commandPtr * sizeof(DrawElementsIndirectCommand), sizeof(DrawElementsIndirectCommand), &cm);
			rendererData.commandPtr++;

			RendererStats.nDrawCalls++;
		}

		void Iaonnis::Renderer3D::drawCommands(Scene* scene, uint32_t program)
		{
			IGPUResource::bindFramebuffer(rendererData.gBuffer);

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(program);

			 glm::mat4 mvp = scene->GetSceneCamera()->getViewProject();

			glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, &mvp[0][0]);

			glBindVertexArray(rendererData.vao);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, rendererData.ibo);
			glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, rendererData.commandPtr, 0);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void Iaonnis::Renderer3D::resetGeometryPtrs()
		{
			rendererData.commandPtr = 0;
			rendererData.currentVertexCount = 0;
			rendererData.currentIndexCount = 0;
			rendererData.indexCount = 0;
			rendererData.vertexCount = 0;  // Add this
			rendererData.materialUploadPtr = 0;  // Add this

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

		void RenderScene(Scene* scene, uint32_t program)
		{
			if (!scene)
			{
				return;
			}

			if (scene->IsEntityRegisteryDirty())
			{
				UploadScene(scene);
				scene->SetEntityRegisteryClean();
			}
			

			drawCommands(scene,program);
			LightPass(scene);
			//EnvironmentPass(scene);

		}

		uint32_t GetRenderOutput()
		{
			return rendererData.fbo.m_Handles[0].m_ID;
		}

		void OnViewFrameResize(Event& event)
		{
			FrameResizeEvent* frameResizeEvent = (FrameResizeEvent*)&event;

			glViewport(0, 0, frameResizeEvent->frameSizeX, frameResizeEvent->frameSizeY);

			FRAMEBUFFER_DESC fboDesc;
			fboDesc.n_Desc = 2;
			fboDesc.textureDesc = (TEXTURE_DESC*)malloc(sizeof(TEXTURE_DESC) * fboDesc.n_Desc);
			fboDesc.textureDesc[0].dataType = TEXTURE_DATA::TEXTURE_COLOR;
			fboDesc.textureDesc[0].height = frameResizeEvent->frameSizeY;
			fboDesc.textureDesc[0].width = frameResizeEvent->frameSizeX;
			fboDesc.textureDesc[0].nBitPerChannel = 32;
			fboDesc.textureDesc[0].nChannels = 4;
			fboDesc.textureDesc[0].ptr = nullptr;
			fboDesc.textureDesc[0].x = 0;
			fboDesc.textureDesc[0].y = 0;

			fboDesc.textureDesc[1].dataType = TEXTURE_DATA::TEXTURE_DEPTH;
			fboDesc.textureDesc[1].height = frameResizeEvent->frameSizeY;
			fboDesc.textureDesc[1].width = frameResizeEvent->frameSizeX;
			fboDesc.textureDesc[1].nBitPerChannel = 24;
			fboDesc.textureDesc[1].nChannels = 1;
			fboDesc.textureDesc[1].ptr = nullptr;
			fboDesc.textureDesc[1].x = 0;
			fboDesc.textureDesc[1].y = 0;

			rendererData.fbo = IGPUResource::resizeFramebuffer(fboDesc, rendererData.fbo);
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
