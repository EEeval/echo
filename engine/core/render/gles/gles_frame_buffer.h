#pragma once

#include "base/buffer/frame_buffer.h"
#include "gles_render_base.h"

namespace Echo
{
	class GLESFrameBufferOffScreen : public FrameBufferOffScreen
	{
	public:
        GLESFrameBufferOffScreen(ui32 width, ui32 height);
		virtual ~GLESFrameBufferOffScreen();

        // begin render
        virtual bool begin() override;
        virtual bool end() override;

        // on resize
        virtual void onSize(ui32 width, ui32 height) override;

		// read pixels
		virtual bool readPixels(Attachment attach, Pixels& pixels) override;

    public:
		// clear render target
		static void clear(bool clear_color, const Color& color, bool clear_depth, float depth_value, bool clear_stencil, ui8 stencil_value);

	protected:
		// Prepare
		bool bind(i32& width, i32& height);

		// Check screen size
		bool checkScreenSize(i32& width, i32& height);

		// Attach render view
		void attach();

		// Specify color buffers will be use
		void specifyColorBuffers();

		// Check frame buffer status
		bool checkFramebufferStatus();

	private:
		GLuint					m_fbo;
		array<GLuint, 9>		m_esTextures;
		vector<GLenum>::type	m_attachments;
	};

	class GLESFramebufferWindow : public FrameBufferWindow
	{
	public:
		GLESFramebufferWindow();
		virtual ~GLESFramebufferWindow();

		// begin render
		virtual bool begin() override;
		virtual bool end() override;

		// on resize
		virtual void onSize(ui32 width, ui32 height) override;

		// read pixels
		virtual bool readPixels(Attachment attach, Pixels& pixels) override;
	};
}
