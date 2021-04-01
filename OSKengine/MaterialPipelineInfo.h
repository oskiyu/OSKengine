#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "MaterialBinding.h"
#include "Renderpass.h"

#include <vector>
#include <string>

namespace OSK {

	enum class PolygonMode {
		FILL,
		LINE
	};

	enum class PolygonCullMode {
		FRONT,
		BACK,
		NONE
	};

	enum class PolygonFrontFaceType {
		CLOCKWISE,
		COUNTERCLOCKWISE
	};

	struct OSKAPI_CALL PushConstantInfo {
		MaterialBindingShaderStage ShaderStage;
		size_t Size;
	};

	struct OSKAPI_CALL MaterialPipelineCreateInfo {
		std::string VertexPath;
		std::string FragmentPath;
		bool CullFaces = true;
		PolygonMode PMode = PolygonMode::FILL;
		PolygonCullMode CullMode = PolygonCullMode::FRONT;
		PolygonFrontFaceType FrontFaceType = PolygonFrontFaceType::CLOCKWISE;
		bool UseDepthStencil = false;
		std::vector<PushConstantInfo> PushConstants;
	};

	VkPolygonMode GetVkPolygonMode(PolygonMode mode);
	VkCullModeFlagBits GetVkCullMode(PolygonCullMode mode);
	VkFrontFace GetVkPolygonFrontFace(PolygonFrontFaceType mode);

}