#pragma once
#include "../Core/Core.h"
#include "../Scene/Camera.h"

namespace Iaonnis
{
	namespace Renderer3D
	{
		static GLuint CreateShaderProgram(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
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
			std::string geometryCode = "";
			if (geometryPath)
			{
				geometryCode = readFile(geometryPath);
			}


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

			GLuint geometryShader;
			if (geometryPath)
			{
				geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
				const char* gSource = geometryCode.c_str();
				glShaderSource(geometryShader, 1, &gSource, nullptr);
				glCompileShader(geometryShader);
				checkShader(geometryShader, "Geometry");
			}

			// Create shader program
			GLuint shaderProgram = glCreateProgram();
			glAttachShader(shaderProgram, vertexShader);
			glAttachShader(shaderProgram, fragmentShader);
			if (geometryPath)
			{
				glAttachShader(shaderProgram, geometryShader);
			}
			glLinkProgram(shaderProgram);
			checkProgram(shaderProgram);

			// Cleanup shaders
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			if (geometryPath)
				glDeleteShader(geometryShader);

			// Check for link errors and return 0 if failed
			GLint linkStatus = GL_FALSE;
			glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
			if (linkStatus != GL_TRUE) {
				glDeleteProgram(shaderProgram);
				return 0;
			}

			return shaderProgram;
		}






		static std::vector<glm::vec4> GetFrustrumCornersWorldSpace(const glm::mat4& viewProj)
        {
            const auto inv = glm::inverse(viewProj);

            std::vector<glm::vec4> frustumCorners;
            for (unsigned int x = 0; x < 2; ++x)
            {
                for (unsigned int y = 0; y < 2; ++y)
                {
                    for (unsigned int z = 0; z < 2; ++z)
                    {
                        const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                        frustumCorners.push_back(pt / pt.w);
                    }
                }
            }

            return frustumCorners;
        }

        static std::vector<glm::vec4> GetFrustrumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
        {
            return GetFrustrumCornersWorldSpace(proj * view);
        }


        static glm::mat4 GetLightSpaceMatrix(std::shared_ptr<Camera> camera, glm::vec3 lightDir, const float& near, const float& far)
        {
            const auto proj = glm::perspective(
                glm::radians(camera->getFrustrum().fov), (float)camera->getFrustrum().x / (float)camera->getFrustrum().y, near,
                far);
            const auto corners = GetFrustrumCornersWorldSpace(proj, camera->getView());

            glm::vec3 center = glm::vec3(0, 0, 0);
            for (const auto& v : corners)
            {
                center += glm::vec3(v);
            }
            center /= corners.size();

            const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

            float minX = std::numeric_limits<float>::max();
            float maxX = std::numeric_limits<float>::lowest();
            float minY = std::numeric_limits<float>::max();
            float maxY = std::numeric_limits<float>::lowest();
            float minZ = std::numeric_limits<float>::max();
            float maxZ = std::numeric_limits<float>::lowest();
            for (const auto& v : corners)
            {
                const auto trf = lightView * v;
                minX = std::min(minX, trf.x);
                maxX = std::max(maxX, trf.x);
                minY = std::min(minY, trf.y);
                maxY = std::max(maxY, trf.y);
                minZ = std::min(minZ, trf.z);
                maxZ = std::max(maxZ, trf.z);
            }

            constexpr float zMult = 10.0f;
            if (minZ < 0)
            {
                minZ *= zMult;
            }
            else
            {
                minZ /= zMult;
            }
            if (maxZ < 0)
            {
                maxZ /= zMult;
            }
            else
            {
                maxZ *= zMult;
            }

            const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
            return lightProjection * lightView;
        }

        static std::vector<glm::mat4> GetLightSpaceMatrices(std::shared_ptr<Camera> camera, glm::vec3 lightDir, float* cascadeLevels,int levelCount)
        {
            float level = 0.0f;
            std::vector<glm::mat4> ret;
            for (size_t i = 0; i < levelCount + 1; ++i)
            {
                level = cascadeLevels[i];
                if (i == 0)
                {
                    ret.push_back(GetLightSpaceMatrix(camera,lightDir, camera->getFrustrum().near, cascadeLevels[i]));
                }
                else if (i < cascadeLevels[i])
                {
                    ret.push_back(GetLightSpaceMatrix(camera, lightDir, cascadeLevels[i - 1], cascadeLevels[i]));
                }
                else
                {
                    ret.push_back(GetLightSpaceMatrix(camera, lightDir, cascadeLevels[i - 1], camera->getFrustrum().far));
                }
            }
            return ret;
        }
	}
}