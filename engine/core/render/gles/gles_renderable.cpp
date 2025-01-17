#include "gles_render_base.h"
#include "gles_renderer.h"
#include "gles_renderable.h"
#include "gles_shader_program.h"
#include "gles_mapping.h"
#include "gles_gpu_buffer.h"
#include <engine/core/util/AssertX.h>
#include <engine/core/util/Exception.h>
#include "engine/core/scene/render_node.h"
#include "base/image/pixel_format.h"
#include "base/Renderer.h"


namespace Echo
{
	extern GLESRenderer* g_renderer;

	GLESRenderable::GLESRenderable()
		: RenderProxy()
	{
	}

	GLESRenderable::~GLESRenderable()
	{
	}

	void GLESRenderable::bindShaderParams()
	{
		ShaderProgram* shaderProgram = m_material->getShader();
		if (shaderProgram)
		{
			i32 textureCount = 0;
			for(ShaderProgram::UniformMap& uniformMap : shaderProgram->getUniforms())
			{
				for (auto& it : uniformMap)
				{
					ShaderProgram::UniformPtr uniform = it.second;
					Material::UniformValue* uniformValue = m_material->getUniform(uniform->m_name);
					if (uniform->m_type != SPT_TEXTURE)
					{
						const void* value = nullptr;
						if (!value && m_camera)
							value = m_camera->getGlobalUniformValue(uniform->m_name);

						if (!value && m_node)
							value = m_node->getGlobalUniformValue(uniform->m_name);

						if (!value)
							value = uniformValue->getValue();

						shaderProgram->setUniform(uniform->m_name.c_str(), value, uniform->m_type, uniform->m_count);
					}
					else
					{
						if (uniformValue)
						{
							Texture* texture = uniformValue->getTexture();
							if (texture)
							{
								Renderer::instance()->setTexture(textureCount, texture);
							}
						}

						shaderProgram->setUniform(uniform->m_name.c_str(), &textureCount, uniform->m_type, uniform->m_count);
						textureCount++;
					}
				}
			}
		}
	}

	void GLESRenderable::setMesh(MeshPtr mesh)
	{
		m_mesh = mesh;

		bindVertexStream();
	}

	void GLESRenderable::setMaterial(Material* material)
	{
		m_material = material;

		bindVertexStream();

		material->onShaderChanged.connectClassMethod(this, createMethodBind(&GLESRenderable::bindVertexStream));
	}

	void GLESRenderable::bindVertexStream()
	{
		if (m_mesh && m_material && m_material->getShader())
		{
			m_vertexStreams.clear();

			StreamUnit unit;
			unit.m_vertElements = m_mesh->getVertexElements();
			unit.m_buffer = m_mesh->getVertexBuffer();
			buildVertStreamDeclaration(&unit);

			m_vertexStreams.emplace_back(unit);
		}
	}

	void GLESRenderable::bind(RenderProxy* pre)
	{
		GPUBuffer* idxBuffer = m_mesh->getIndexBuffer();
		GPUBuffer* preIdxBuffer = pre && pre->getNode() ? pre->getMesh()->getIndexBuffer() : nullptr;

		// 1. is need set vertex buffer
		bool isNeedSetVertexBuffer;
		bool isNeedSetIdxBuffer;
		if (!pre)
		{
			isNeedSetVertexBuffer = true;
			isNeedSetIdxBuffer = idxBuffer ? true : false;
		}
		else
		{
#ifdef ECHO_PLATFORM_IOS
            isNeedSetVertexBuffer = true;//pre->getVertexStreamHash() != m_vertexStreamsHash ? true : false;
#else
			isNeedSetVertexBuffer = true;
#endif
			isNeedSetIdxBuffer = (!idxBuffer || preIdxBuffer == idxBuffer) ? false : true;
		}

		// bind vertex stream
		if (isNeedSetVertexBuffer)
		{
			for (i32 i = (i32)(m_vertexStreams.size() - 1); i >= 0; i--)
			{
				const StreamUnit& streamUnit = m_vertexStreams[i];

				((GLESGPUBuffer*)streamUnit.m_buffer)->bindBuffer();

				size_t declarationSize = streamUnit.m_vertDeclaration.size();
				for (size_t i = 0; i < declarationSize; ++i)
				{
					const VertexDeclaration& declaration = streamUnit.m_vertDeclaration[i];
					if (declaration.m_attribute != -1)
					{
						// Enable the vertex array attributes.
						OGLESDebug(glVertexAttribPointer(declaration.m_attribute, declaration.count, declaration.type, declaration.bNormalize, streamUnit.m_vertStride, (GLvoid*)declaration.elementOffset));
						g_renderer->enableAttribLocation(declaration.m_attribute);
					}
				}
			}
		}

		// bind index buffer
		if ( isNeedSetIdxBuffer)
		{
			// Bind the index buffer and load the index data into it.
			((GLESGPUBuffer*)idxBuffer)->bindBuffer();
		}
	}

	// unbind
	void GLESRenderable::unbind()
	{
#ifndef ECHO_PLATFORM_IOS
		// bind vertex stream
  		for (size_t i = 0; i < m_vertexStreams.size(); i++)
  		{
  			const StreamUnit& streamUnit = m_vertexStreams[i];
  			size_t declarationSize = streamUnit.m_vertDeclaration.size();
  			for (size_t i = 0; i < declarationSize; ++i)
  			{
  				const VertexDeclaration& declaration = streamUnit.m_vertDeclaration[i];
 				if (declaration.m_attribute != -1)
 				{
 					// Enable the vertex array attributes.
					g_renderer->disableAttribLocation(declaration.m_attribute);
 				}
  			}
  		}
#endif
	}

	bool GLESRenderable::buildVertStreamDeclaration(StreamUnit* stream)
	{
		ui32 numVertElms = static_cast<ui32>(stream->m_vertElements.size());
		if (numVertElms == 0)
		{
			EchoLogError("Vertex elements size error, buildVertStreamDeclaration failed.");
			return false;
		}

		stream->m_vertDeclaration.reserve(numVertElms);
		stream->m_vertDeclaration.resize(numVertElms);

		GLESShaderProgram* gles2Program = ECHO_DOWN_CAST<GLESShaderProgram*>(m_material->getShader());
		ui32 elmOffset = 0;
		for (size_t i = 0; i < numVertElms; ++i)
		{
			stream->m_vertDeclaration[i].m_attribute = gles2Program->getAtrribLocation(stream->m_vertElements[i].m_semantic);
			stream->m_vertDeclaration[i].count = PixelUtil::GetChannelCount(stream->m_vertElements[i].m_pixFmt);
			stream->m_vertDeclaration[i].type = GLESMapping::MapDataType(stream->m_vertElements[i].m_pixFmt);
			stream->m_vertDeclaration[i].bNormalize = PixelUtil::IsNormalized(stream->m_vertElements[i].m_pixFmt);
			stream->m_vertDeclaration[i].elementOffset = elmOffset;
			elmOffset += PixelUtil::GetPixelBytes(stream->m_vertElements[i].m_pixFmt);
		}

		// check
		for (i32 i = 0; i < VS_MAX; ++i)
		{
			i32 loc = gles2Program->getAtrribLocation((VertexSemantic)i);
			if (loc >= 0)
			{
				bool found = false;
				for (size_t size = 0; size < numVertElms; ++size)
				{
					if (stream->m_vertElements[size].m_semantic == i)
					{
						found = true;
						break;
					}
				}

				if (!found)
				{
					String errorInfo = StringUtil::Format("Vertex Attribute [%s] name is NOT in Vertex Stream", GLESMapping::MapVertexSemanticString((VertexSemantic)i).c_str());
					EchoLogFatal(errorInfo.c_str());
					EchoAssertX(false, errorInfo.c_str());
				}
			}
		}

		stream->m_vertStride = elmOffset;

		return true;
	}

	void GLESRenderable::bindRenderState()
	{
		ShaderProgram* shaderProgram = m_material->getShader();
		if (shaderProgram)
		{
			GLESRenderer* glesRenderer = (ECHO_DOWN_CAST<GLESRenderer*>(Renderer::instance()));
			glesRenderer->setDepthStencilState(shaderProgram->getDepthStencilState());
			glesRenderer->setRasterizerState(m_material->getRasterizerState());
			glesRenderer->setBlendState(shaderProgram->getBlendState());
		}
	}
}
