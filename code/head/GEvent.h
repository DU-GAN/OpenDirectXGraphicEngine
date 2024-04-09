#pragma once

#include "UString.h"

namespace Rendering
{
	class GEvent
	{
	public:
		enum Type
		{
			None,
			UpdateMSAAState,
			UpdateFullScreen,
			UpdateFrameResource,
			LoadModel,
			UpdateRenderMode,
			UpdateCullBackFace,
		};

		GEvent(Type _type = None) :type(_type) {}

		Type type;

		bool b1, b2;
		int i1, i2;
		float f1, f2;
		UString s1, s2;
	};
}