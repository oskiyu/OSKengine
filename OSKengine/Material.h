#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "MaterialLayout.h"
#include "MaterialInstance.h"

#include "GraphicsPipeline.h"
#include "VulkanRenderer.h"

namespace OSK {

	class OSKAPI_CALL Material {

	public:

		void Create() {
			GPipeline = Renderer->CreateNewGraphicsPipeline(VertexPath, FragmentPath);
		}

	private:

		std::string VertexPath;
		std::string FragmentPath;

		GraphicsPipeline* GPipeline = nullptr;
		RenderAPI* Renderer = nullptr;

	};

}