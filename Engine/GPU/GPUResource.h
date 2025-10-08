#pragma once
#include "../Core/pch.h"
#include "../Core/core.h"

namespace Iaonnis {
	enum FBO_FLAGS : uint32_t
	{
		DEPTH_NO_COLOR = 1 << 1, //glDrawBuffer(NONE)
		ALL_COLOR_ATTACHMENT = 1 << 2 //Use all color attachmanents
	};

	enum UBO_SLOTS
	{
		CAMERA_DATA = 0,
		MATERIAL_SLOT = 1,
		LIGHT_SLOT = 2
	};

	struct Handle
	{
		uint32_t m_ID;
	};
	enum BUFFER_TYPE
	{
		UNIFORM_BUFFER,
		ARRAY_BUFFER,
		ELEMENT_ARRAY_BUFFER,
	};

	struct UPLOAD_BUFFER_DESC
	{
		void* ptr;

		/// @brief in bytes
		size_t size;
		size_t offset;

		int slot; ///@brief current usage UBO range binding

		BUFFER_TYPE bufferTarget;
	};

	enum class TEXTURE_DATA
	{
		TEXTURE_COLOR,
		TEXTURE_DEPTH
	};

	struct ResourceBlob
	{
		const char* blob;
	};

	struct SHADER_DESC
	{
		ResourceBlob vertexBlob;
		ResourceBlob fragmentBlob;
	};

	enum class SHADER_UPLOAD_TYPE
	{
		FLOAT, INT,
		VEC2, VEC3, VEC4,
		MAT3, MAT4, BOOL,
		UBO, SAMPLER2D
	};

	/// @brief 
	struct SHADER_UPLOAD_DESC
	{
		SHADER_UPLOAD_TYPE dataType;
		const char* name;
		void* ptr;
		/// @brief index param is used for ubo and sampler2D uploads
		int index;
	};

	struct ShaderHandle : public Handle
	{
		bool valid;
	private:
		friend class IGPUResource;
		uint32_t vertexID;
		uint32_t fragmentID;
	};

	struct TEXTURE_DESC
	{
		int x;
		int y;
		int nChannels;
		uint8_t nBitPerChannel;

		int width;
		int height;

		void* ptr;

		TEXTURE_DATA dataType = TEXTURE_DATA::TEXTURE_COLOR;
	};

	struct FRAMEBUFFER_DESC
	{
		TEXTURE_DESC* textureDesc;
		int n_Desc;

		~FRAMEBUFFER_DESC()
		{
			if (textureDesc)
			{
			}
		}
	};

	/// <summary>
	/// m_ID is the texture id for the texture buffer.
	/// handle is the GLuint64 handle returned by the GPU after the texture was made resident.
	/// </summary>
	struct TextureHandle : public Handle
	{
		TEXTURE_DATA dataType;

		GLuint64 handle;
	};

	struct FramebufferHandle : public Handle
	{
		TextureHandle* m_Handles;
		int nHandles;

		int flags;
	};

	struct UniformbufferHandle : public Handle {};
	struct CubeMapHandle : public Handle {};

	class IGPUResource
	{
	public:
		static TextureHandle createGPUTexture(TEXTURE_DESC desc);
		static void fillTexture(TextureHandle handle, TEXTURE_DESC desc);
		static void reallocTexture(TextureHandle handle, TEXTURE_DESC desc);//don't use
		static void destroyTexture(TextureHandle handle);
		static void getTexturePixels(TextureHandle handle, TEXTURE_DESC& desc);
		static void bindTextureToSlot(TextureHandle handle, int slot);

		static FramebufferHandle createFrambuffer(FRAMEBUFFER_DESC desc, int creationFlags = 0);
		static void DeleteFramebuffer(FramebufferHandle handle);
		static FramebufferHandle resizeFramebuffer(FRAMEBUFFER_DESC desc, FramebufferHandle oldHandle);
		static void resizeFramebufferTextures(FRAMEBUFFER_DESC desc, FramebufferHandle& handle);
		static void bindFramebuffer(FramebufferHandle handle);

		static ShaderHandle createShader(SHADER_DESC desc);
		static void destroyShader(ShaderHandle handle);

		static void useShader(ShaderHandle handle);
		static int  getUniformLocation(ShaderHandle handle, const char* name);
		static int  getUniformBlockIndex(ShaderHandle handle, const char* name);


		static bool uploadUniform(ShaderHandle handle, SHADER_UPLOAD_DESC* uploadDescs, int nDesc);

		static UniformbufferHandle createUniformbuffer(UPLOAD_BUFFER_DESC desc);
		static void uploadBufferData(Handle handle, UPLOAD_BUFFER_DESC* desc, int nDesc);
		static void bindBufferRange(Handle handle, UPLOAD_BUFFER_DESC desc);

		static CubeMapHandle createCubeMap(TEXTURE_DESC* desc);

		static void check_errors();

	};
}