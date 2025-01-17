#include "pcg_image.h"

namespace Echo
{
	PCGImage::PCGImage()
	{

	}

	PCGImage::~PCGImage()
	{

	}

	void PCGImage::set(i32 width, i32 height)
	{
		m_width = width;
		m_height = height;
		m_colors.resize(m_width * m_height, Color::BLACK);
	}

	void PCGImage::setValue(i32 x, i32 y, const Color& color)
	{
		if (x >= 0 && x < m_width && y >= 0 && y < m_height)
		{
			i32 index = y * m_width + x;
			m_colors[index] = color;
		}
	}

	const Color& PCGImage::getValue(i32 x, i32 y) const
	{ 
		i32 index = y * m_width + x;
		return m_colors[index];
	}
}
