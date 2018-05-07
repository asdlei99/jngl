// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "dll.hpp"

namespace jngl {
	class FrameBufferImpl;

	class JNGLDLL_API FrameBuffer {
	public:
		FrameBuffer(int width, int height);
		~FrameBuffer();
		void beginDraw();
		void endDraw();
		void draw(double x, double y) const;
		void clear();
	private:
		FrameBuffer(const FrameBuffer&);
		FrameBuffer& operator=(const FrameBuffer&);
		FrameBufferImpl* pImpl;
	};
}
