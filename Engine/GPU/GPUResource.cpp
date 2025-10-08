#include "GPUResource.h"

namespace Iaonnis {
	GLenum getInternalFormat(int nChannel, uint8_t nBitPerChannel, TEXTURE_DATA dataType)
	{
		if (dataType == TEXTURE_DATA::TEXTURE_COLOR)
		{
			switch (nBitPerChannel)
			{
			case 8:
				switch (nChannel)
				{
				case 1: return GL_R8;
				case 2: return GL_RG8;
				case 3: return GL_RGB8;
				case 4: return GL_RGBA8;
				}
				break;

			case 16:
				switch (nChannel)
				{
				case 1: return GL_R16F;
				case 2: return GL_RG16F;
				case 3: return GL_RGB16F; 
				case 4: return GL_RGBA16F;
				}
				break;

			case 32:
				switch (nChannel)
				{
				case 1: return GL_R32F;
				case 2: return GL_RG32F;
				case 3: return GL_RGB32F;
				case 4: return GL_RGBA32F;
				}
				break;
			}
		}

		else if (dataType == TEXTURE_DATA::TEXTURE_DEPTH)
		{
			switch (nBitPerChannel)
			{
			case 16: return GL_DEPTH_COMPONENT16;
			case 24: return GL_DEPTH_COMPONENT24;
			case 32: return GL_DEPTH_COMPONENT32;
			default: return GL_DEPTH_COMPONENT32F;
			}
		}

		return GL_RGBA8;
	}


	GLenum getFormat(int nChannel, TEXTURE_DATA dataType)
	{
		if (dataType == TEXTURE_DATA::TEXTURE_COLOR)
		{
			switch (nChannel)
			{
			case 1: return GL_RED;
			case 2: return GL_RG;
			case 3: return GL_RGB;
			case 4: return GL_RGBA;
			default: return GL_RGB;
			}
		}
		return GL_DEPTH_COMPONENT;
	}

	GLenum getChannelType(uint8_t nBits)
	{
		switch (nBits)
		{
		case  8: return GL_UNSIGNED_BYTE;
		case 16: return GL_SHORT;
		case 32: return GL_FLOAT;
		default: return GL_UNSIGNED_BYTE;
		}
	}

	GLenum getAttachmentType(TEXTURE_DATA dataType)
	{
		switch (dataType)
		{
		case TEXTURE_DATA::TEXTURE_COLOR: return GL_COLOR_ATTACHMENT0;
		case TEXTURE_DATA::TEXTURE_DEPTH: return GL_DEPTH_ATTACHMENT;
		default: return GL_COLOR_ATTACHMENT0;
		}
	}

	GLenum getBufferTarget(BUFFER_TYPE bufferType)
	{
		switch (bufferType)
		{
		case BUFFER_TYPE::UNIFORM_BUFFER:return GL_UNIFORM_BUFFER;
		case BUFFER_TYPE::ARRAY_BUFFER:return GL_ARRAY_BUFFER;
		case BUFFER_TYPE::ELEMENT_ARRAY_BUFFER: return GL_ELEMENT_ARRAY_BUFFER;
		}
	}

	TextureHandle IGPUResource::createGPUTexture(TEXTURE_DESC desc)
	{
		TextureHandle rHandle;
		glGenTextures(1, &rHandle.m_ID);
		glBindTexture(GL_TEXTURE_2D, rHandle.m_ID);

		glTexImage2D(GL_TEXTURE_2D, 0, getInternalFormat(desc.nChannels, desc.nBitPerChannel, desc.dataType), desc.width, desc.height, 0, getFormat(desc.nChannels, desc.dataType), getChannelType(desc.nBitPerChannel), desc.ptr);

		//todo: provide interface to properly set these values.
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		rHandle.handle = glGetTextureHandleARB(rHandle.m_ID);
		IAONNIS_ASSERT((rHandle.handle != 0), "Invalid texture handles.");
		glMakeTextureHandleResidentARB(rHandle.handle);

		return rHandle;
	}

	void IGPUResource::fillTexture(TextureHandle handle, TEXTURE_DESC desc)
	{
		glBindTexture(GL_TEXTURE_2D, handle.m_ID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, desc.x, desc.y, desc.width, desc.height, getFormat(desc.nChannels, desc.dataType), getChannelType(desc.nBitPerChannel), desc.ptr);
	}

	void IGPUResource::reallocTexture(TextureHandle handle, TEXTURE_DESC desc)
	{
		glBindTexture(GL_TEXTURE_2D, handle.m_ID);
		glTexImage2D(GL_TEXTURE_2D, 0, getInternalFormat(desc.nChannels, desc.nBitPerChannel, desc.dataType), desc.width, desc.height, 0, getFormat(desc.nChannels, desc.dataType), getChannelType(desc.nBitPerChannel), desc.ptr);
	}

	void IGPUResource::destroyTexture(TextureHandle handle)
	{
		IAONNIS_ASSERT((handle.handle != 0), "Invalid texture handles.");
		glMakeTextureHandleNonResidentARB(handle.handle);
		glDeleteTextures(1, &handle.m_ID);
	}

	void IGPUResource::getTexturePixels(TextureHandle handle, TEXTURE_DESC& desc)
	{
		glBindTexture(GL_TEXTURE_2D, handle.m_ID);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		void* r;
		switch (desc.nBitPerChannel)
		{
			case 8:
			{
				desc.ptr = (unsigned char*)malloc(sizeof(unsigned char) * desc.width * desc.height * desc.nChannels);
				break;
			}
			case 16:
			{
				desc.ptr = (unsigned short*)malloc(sizeof(unsigned short) * desc.width * desc.height * desc.nChannels);
				break;
			}
		}
		glGetTexImage(GL_TEXTURE_2D, 0, getFormat(desc.nChannels, desc.dataType), getChannelType(desc.nBitPerChannel), desc.ptr);
		return;
	}

	void IGPUResource::bindTextureToSlot(TextureHandle handle, int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, handle.m_ID);
	}

	FramebufferHandle IGPUResource::createFrambuffer(FRAMEBUFFER_DESC desc, int creationFlags)
	{
		FramebufferHandle rHandle;
		glGenFramebuffers(1, &rHandle.m_ID);
		glBindFramebuffer(GL_FRAMEBUFFER, rHandle.m_ID);

		rHandle.m_Handles = (TextureHandle*)malloc(sizeof(TextureHandle) * desc.n_Desc);
		rHandle.nHandles = desc.n_Desc;

		int colorAttachmentCount = 0;
		GLenum activeAttachments[15];

		for (int i = 0; i < desc.n_Desc; i++)
		{
			rHandle.m_Handles[i] = createGPUTexture(desc.textureDesc[i]);

			auto attachmentType = getAttachmentType(desc.textureDesc[i].dataType);
			if (desc.textureDesc[i].dataType == TEXTURE_DATA::TEXTURE_COLOR)
			{
				attachmentType += colorAttachmentCount;
				activeAttachments[colorAttachmentCount] = attachmentType;
				colorAttachmentCount++;
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, rHandle.m_Handles[i].m_ID, 0);
		}

		if (creationFlags & FBO_FLAGS::DEPTH_NO_COLOR)
		{
			glDrawBuffer(GL_NONE);
		}
		else if (creationFlags & FBO_FLAGS::ALL_COLOR_ATTACHMENT)
		{
			glDrawBuffers(colorAttachmentCount, activeAttachments);
		}

		rHandle.flags = 0;
		rHandle.flags |= creationFlags;

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			IAONNIS_LOG_ERROR("[GPU RESOURCE]: Framebuffer incomplete: 0x%X", status);
			// consider cleaning up resources here
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return rHandle;
	}

	void Iaonnis::IGPUResource::DeleteFramebuffer(FramebufferHandle handle)
	{
		glDeleteFramebuffers(1, &handle.m_ID);
		for (int i = 0; i < handle.nHandles; i++)
		{
			glDeleteTextures(1, &handle.m_Handles[i].m_ID);
		}
	}

	FramebufferHandle IGPUResource::resizeFramebuffer(FRAMEBUFFER_DESC desc, FramebufferHandle oldHandle)
	{
		DeleteFramebuffer(oldHandle);

		FramebufferHandle rHandle = createFrambuffer(desc, oldHandle.flags);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			IAONNIS_LOG_ERROR("[GPU RESOURCE]: Framebuffer incomplete: 0x%X", status);
		}

		return rHandle;
	}

	void IGPUResource::resizeFramebufferTextures(FRAMEBUFFER_DESC desc, FramebufferHandle& handle)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, handle.m_ID);

		for (int i = 0; i < desc.n_Desc; i++)
		{
			destroyTexture(handle.m_Handles[i]);
		}

		for (int i = 0; i < desc.n_Desc; i++)
		{
			handle.m_Handles[i] = createGPUTexture(desc.textureDesc[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, getAttachmentType(desc.textureDesc[i].dataType), GL_TEXTURE_2D, handle.m_Handles[i].m_ID, 0);
		}

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			IAONNIS_LOG_ERROR("[GPU RESOURCE]: Framebuffer incomplete: 0x%X", status);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

	void IGPUResource::bindFramebuffer(FramebufferHandle handle)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, handle.m_ID);
	}

	ShaderHandle IGPUResource::createShader(SHADER_DESC desc)
	{
		ShaderHandle rHandle;

		rHandle.vertexID = glCreateShader(GL_VERTEX_SHADER);
		rHandle.fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(rHandle.vertexID, 1, &desc.vertexBlob.blob, nullptr);
		glShaderSource(rHandle.fragmentID, 1, &desc.fragmentBlob.blob, nullptr);

		glCompileShader(rHandle.vertexID);
		glCompileShader(rHandle.fragmentID);

		rHandle.m_ID = glCreateProgram();
		glAttachShader(rHandle.m_ID, rHandle.vertexID);
		glAttachShader(rHandle.m_ID, rHandle.fragmentID);

		glLinkProgram(rHandle.m_ID);

		rHandle.valid = false;

		int success;
		char infoLog[512];
		glGetShaderiv(rHandle.vertexID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(rHandle.vertexID, 512, NULL, infoLog);
			IAONNIS_LOG_ERROR("[GPU RESOURCE]: Vertex Shader Compilation Error.\n%s", infoLog);
			return rHandle;
		}

		glGetShaderiv(rHandle.fragmentID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(rHandle.fragmentID, 512, NULL, infoLog);
			IAONNIS_LOG_ERROR("[GPU RESOURCE]: Fragment Shader Compilation Error.\n%s", infoLog);
			return rHandle;
		}
		glGetProgramiv(rHandle.m_ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(rHandle.m_ID, 512, NULL, infoLog);
			IAONNIS_LOG_ERROR("[GPU RESOURCE]: Prgram Linking Error.\n%s", infoLog);
			return rHandle;
		}

		glUseProgram(0);
		rHandle.valid = true;
		return rHandle;
	}

	void IGPUResource::destroyShader(ShaderHandle handle)
	{
		glDeleteShader(handle.fragmentID);
		glDeleteShader(handle.vertexID);
		glDeleteProgram(handle.m_ID);
	}

	void IGPUResource::useShader(ShaderHandle handle)
	{
		glUseProgram(handle.m_ID);
	}

	int IGPUResource::getUniformLocation(ShaderHandle handle, const char* name)
	{
		return glGetUniformLocation(handle.m_ID, name);
	}

	int IGPUResource::getUniformBlockIndex(ShaderHandle handle, const char* name)
	{
		return glGetUniformBlockIndex(handle.m_ID, name);
	}

	bool IGPUResource::uploadUniform(ShaderHandle handle, SHADER_UPLOAD_DESC* uploadDescs, int nDesc)
	{
		bool badUpload = false;
		for (int i = 0; i < nDesc; i++)
		{
			int location = getUniformLocation(handle, uploadDescs[i].name);
			if (location == -1)
			{
				badUpload = true;
			}

			switch (uploadDescs[i].dataType)
			{
			case SHADER_UPLOAD_TYPE::FLOAT:
				glUniform1f(location, *static_cast<float*>(uploadDescs[i].ptr));
				break;
			case SHADER_UPLOAD_TYPE::INT:
				glUniform1i(location, *static_cast<int*>(uploadDescs[i].ptr));
				break;
			case SHADER_UPLOAD_TYPE::VEC2:
				glUniform2fv(location, 1, static_cast<float*>(uploadDescs[i].ptr));
				break;
			case SHADER_UPLOAD_TYPE::VEC3:
				glUniform3fv(location, 1, static_cast<float*>(uploadDescs[i].ptr));
				break;
			case SHADER_UPLOAD_TYPE::VEC4:
				glUniform4fv(location, 1, static_cast<float*>(uploadDescs[i].ptr));
				break;
			case SHADER_UPLOAD_TYPE::MAT3:
				glUniformMatrix3fv(location, 1, GL_FALSE, static_cast<float*>(uploadDescs[i].ptr));
				break;
			case SHADER_UPLOAD_TYPE::MAT4:
				glUniformMatrix4fv(location, 1, GL_FALSE, static_cast<float*>(uploadDescs[i].ptr));
				break;
			case SHADER_UPLOAD_TYPE::BOOL:
				glUniform1i(location, *static_cast<bool*>(uploadDescs[i].ptr));
				break;
			case SHADER_UPLOAD_TYPE::UBO:
			{
				location = getUniformBlockIndex(handle, uploadDescs[i].name);
				if (location == GL_INVALID_INDEX)
				{
					return false;
				}
				glUniformBlockBinding(handle.m_ID, location, uploadDescs[i].index);
				break;
			}
			case SHADER_UPLOAD_TYPE::SAMPLER2D:
				glUniform1i(location, uploadDescs[i].index);
				break;
			default:
				return !badUpload;
			}
		}

		return !badUpload;
	}

	UniformbufferHandle IGPUResource::createUniformbuffer(UPLOAD_BUFFER_DESC desc)
	{
		UniformbufferHandle rHandle;

		glGenBuffers(1, &rHandle.m_ID);
		glBindBuffer(GL_UNIFORM_BUFFER, rHandle.m_ID);
		glBufferData(GL_UNIFORM_BUFFER, desc.size, desc.ptr, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		return rHandle;
	}

	void IGPUResource::bindBufferRange(Handle handle, UPLOAD_BUFFER_DESC desc)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, handle.m_ID);
		glBindBufferRange(GL_UNIFORM_BUFFER, desc.slot, handle.m_ID, 0, desc.size);
	}

	CubeMapHandle Iaonnis::IGPUResource::createCubeMap(TEXTURE_DESC* desc)
	{
		CubeMapHandle handle;
		glGenTextures(1, &handle.m_ID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, handle.m_ID);

		for (int i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
				getInternalFormat(desc[i].nChannels, desc[i].nBitPerChannel, desc[i].dataType),
				desc[i].width, desc[i].height, 0, getFormat(desc[i].nChannels, desc[i].dataType),
				getChannelType(desc[i].nBitPerChannel), desc[i].ptr);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
			GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
			GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
			GL_CLAMP_TO_EDGE);

		return handle;
	}

	void IGPUResource::check_errors()
	{

	}

	/// @brief Do not upload buffer data for different handle.
	void IGPUResource::uploadBufferData(Handle handle, UPLOAD_BUFFER_DESC* desc, int nDesc)
	{
		glBindBuffer(getBufferTarget(desc->bufferTarget), handle.m_ID);

		for (int i = 0; i < nDesc; i++)
		{
			glBufferSubData(getBufferTarget(desc[i].bufferTarget), desc[i].offset, desc[i].size, desc[i].ptr);
		}
	}
}