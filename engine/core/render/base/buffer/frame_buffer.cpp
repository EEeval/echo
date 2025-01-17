#include "base/renderer.h"
#include "base/buffer/frame_buffer.h"
#include "base/shader/shader_program.h"
#include "engine/core/main/Engine.h"

namespace Echo
{
	void FrameBuffer::bindMethods()
	{
		CLASS_BIND_METHOD(FrameBuffer, isClearColor);
		CLASS_BIND_METHOD(FrameBuffer, setClearColor);
		CLASS_BIND_METHOD(FrameBuffer, getClearColorValue);
		CLASS_BIND_METHOD(FrameBuffer, setClearColorValue);
		CLASS_BIND_METHOD(FrameBuffer, isClearDepth);
		CLASS_BIND_METHOD(FrameBuffer, setClearDepth);

		CLASS_REGISTER_PROPERTY(FrameBuffer, "IsClearColor", Variant::Type::Bool, isClearColor, setClearColor);
		CLASS_REGISTER_PROPERTY(FrameBuffer, "ClearColor", Variant::Type::Color, getClearColorValue, setClearColorValue);
		CLASS_REGISTER_PROPERTY(FrameBuffer, "IsClearDepth", Variant::Type::Bool, isClearDepth, setClearDepth);
	}

	FrameBufferOffScreen::FrameBufferOffScreen()
    {
        m_views.assign(nullptr);
    }

	FrameBufferOffScreen::FrameBufferOffScreen(ui32 width, ui32 height)
	{
        m_views.assign(nullptr);
	}

	FrameBufferOffScreen::~FrameBufferOffScreen()
	{
	}

	void FrameBufferOffScreen::bindMethods()
	{
		CLASS_BIND_METHOD(FrameBufferOffScreen, getColorA);
		CLASS_BIND_METHOD(FrameBufferOffScreen, setColorA);
		CLASS_BIND_METHOD(FrameBufferOffScreen, getColorB);
		CLASS_BIND_METHOD(FrameBufferOffScreen, setColorB);
		CLASS_BIND_METHOD(FrameBufferOffScreen, getDepth);
		CLASS_BIND_METHOD(FrameBufferOffScreen, setDepth);

		CLASS_REGISTER_PROPERTY(FrameBufferOffScreen, "ColorA", Variant::Type::ResourcePath, getColorA, setColorA);
		CLASS_REGISTER_PROPERTY(FrameBufferOffScreen, "ColorB", Variant::Type::ResourcePath, getColorB, setColorB);
		CLASS_REGISTER_PROPERTY(FrameBufferOffScreen, "DepthStencil", Variant::Type::ResourcePath, getDepth, setDepth);
	}

	Res* FrameBufferOffScreen::create()
	{
		return Renderer::instance()->createFrameBufferOffScreen(Renderer::instance()->getWindowWidth(), Renderer::instance()->getWindowHeight());
	}

	ResourcePath FrameBufferOffScreen::getColorA()
	{
		return m_views[0] ? ResourcePath(m_views[0]->getPath(), ".rt") : ResourcePath("", ".rt");
	}

	void FrameBufferOffScreen::setColorA(const ResourcePath& path)
	{
		m_views[0] = ECHO_DOWN_CAST<TextureRenderTarget2D*>(Res::get(path));
	}

	ResourcePath FrameBufferOffScreen::getColorB()
	{
		return m_views[Attachment::ColorB] ? ResourcePath(m_views[Attachment::ColorB]->getPath(), ".rt") : ResourcePath("", ".rt");
	}

	void FrameBufferOffScreen::setColorB(const ResourcePath& path)
	{
		m_views[Attachment::ColorB] = ECHO_DOWN_CAST<TextureRenderTarget2D*>(Res::get(path));
	}

	ResourcePath FrameBufferOffScreen::getDepth()
	{
		i32 idx = i32(Attachment::DepthStencil);
		return m_views[idx] ? ResourcePath(m_views[idx]->getPath(), ".rt") : ResourcePath("", ".rt");
	}

	void FrameBufferOffScreen::setDepth(const ResourcePath& path)
	{
		i32 idx = i32(Attachment::DepthStencil);
		m_views[idx] = ECHO_DOWN_CAST<TextureRenderTarget2D*>(Res::get(path));
	}

	Res* FrameBufferWindow::create()
	{
		return Renderer::instance()->createFrameBufferWindow();
	}

	void FrameBufferWindow::bindMethods()
	{
	}
}
