#pragma once

#include "RenderItemWrap.h"

namespace Rendering
{
	class RenderResourceManager
	{
	public:
		RenderResourceManager()
		{

		}

		inline std::vector<RenderPipline>& GetPiplines()
		{
			return piplines;
		}

		inline std::vector<Shader>& GetShaders()
		{
			return shaders;
		}

		inline std::vector<RenderItemWrap>& GetRenderItemWraps()
		{
			return renderItemsWraps;
		}

	private:
		std::vector<RenderItemWrap> renderItemsWraps;
		std::vector<RenderPipline> piplines;
		std::vector<Shader> shaders;
	};
}