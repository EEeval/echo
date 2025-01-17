#include "render_pipeline.h"
#include "base/renderer.h"
#include "base/buffer/frame_buffer.h"
#include "engine/core/io/IO.h"
#include "render_stage.h"
#include <thirdparty/pugixml/pugixml.hpp>

namespace Echo
{
	const ResourcePath RenderPipeline::DefaultPipeline = ResourcePath("Engine://Render/Pipeline/DeferredShading.pipeline");
	static RenderPipelinePtr g_current;

	RenderPipeline::RenderPipeline()
	{
	}

	RenderPipeline::RenderPipeline(const ResourcePath& path)
		: Res(path)
	{
	}

	RenderPipeline::~RenderPipeline()
	{
	}

	void RenderPipeline::bindMethods()
	{
	}

	RenderPipelinePtr RenderPipeline::current()
	{
		if (!g_current)
		{
			setCurrent(DefaultPipeline);
		}

		return g_current;
	}

	void RenderPipeline::setCurrent(const ResourcePath& path)
	{
		g_current = nullptr;

		if(!path.isEmpty())
			g_current = ECHO_DOWN_CAST<RenderPipeline*>(Res::get(path));
	}

	void RenderPipeline::setSrc(const String& src)
	{
		m_srcData = src;

		if (!m_isParsed)
		{
			parseXml();

			m_isParsed = true;
		}
	}

	void RenderPipeline::addStage(RenderStage* stage, ui32 position)
	{
		if (position < m_stages.size())
		{
			m_stages.insert(m_stages.begin() + position, stage);
		}
		else
		{
			m_stages.emplace_back(stage);
		}
	}

	void RenderPipeline::deleteStage(RenderStage* stage)
	{
		for (auto it = m_stages.begin(); it != m_stages.end(); it++)
		{
			if (*it == stage)
			{
				EchoSafeDelete(stage, RenderStage);
				it = m_stages.erase(it);

				break;
			}
		}
	}

	void RenderPipeline::onSize(ui32 width, ui32 height)
	{
		for (RenderStage* stage : m_stages)
		{
			stage->onSize(width, height);
		}
	}

	void RenderPipeline::addRenderable(const String& name, RenderableID id)
	{
		for (RenderStage* stage : m_stages)
		{
			stage->addRenderable(name, id);
		}
	}

	void RenderPipeline::render()
	{
        for (RenderStage* stage : m_stages)
        {
            stage->render();
        }
        
        Renderer::instance()->present();
	}

	void RenderPipeline::parseXml()
	{
		EchoSafeDeleteContainer(m_stages, RenderStage);

		pugi::xml_document doc;
		if (doc.load_buffer(m_srcData.data(), m_srcData.size()));
		{
			pugi::xml_node rootNode = doc.child("pipeline");
			if (rootNode)
			{
				for (pugi::xml_node stageNode = rootNode.child("stage"); stageNode; stageNode = stageNode.next_sibling("stage"))
				{
					RenderStage* stage = ECHO_DOWN_CAST<RenderStage*>(instanceObject(&stageNode));
					stage->setPipeline(this);
					stage->parseXml(&stageNode);
					m_stages.emplace_back(stage);
				}
			}
		}
	}

	Res* RenderPipeline::load(const ResourcePath& path)
	{
		MemoryReader reader(path.getPath());
		if (reader.getSize())
		{
			RenderPipeline* res = EchoNew(RenderPipeline(path));
			res->setSrc(reader.getData<const char*>());

			return res;
		}

		return nullptr;
	}

	void RenderPipeline::save()
	{
		pugi::xml_document doc;

		pugi::xml_node rootNode = doc.append_child("pipeline");
		for (RenderStage* stage : m_stages)
		{
			stage->saveXml(&rootNode);
		}

		String fullPath = IO::instance()->convertResPathToFullPath(m_path.getPath());
		doc.save_file(fullPath.c_str(), "\t", 1U, pugi::encoding_utf8);
	}
}
