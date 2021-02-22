#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"

#include "GraphicsPipeline.h"
#include "DescriptorLayout.h"
#include "Renderpass.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "ShaderBindingType.h"
#include "ShaderStage.h"

namespace OSK {


	class OSKAPI_CALL MaterialInstance {

		friend class Material;

	public:

	private:

		DescriptorSet* Descriptor = nullptr;

	};


	class OSKAPI_CALL Material {

	public:

		void SetRenderer(RenderAPI* renderer);

		void SetShaders(const Shader& shader);
		void SetRenderTarget(RenderTarget* target);

		void AddBinding(ShaderBindingType type, ShaderStage stage);
		void AddVertexBinding(ShaderBindingType type);
		void AddFragmentBinding(ShaderBindingType type);

		void Create();
		void CreateInstance(MaterialInstance* instance);

	private:

		RenderAPI* renderer = nullptr;

		GraphicsPipeline* GPipeline = nullptr;
		DescriptorLayout* Layout = nullptr;
		RenderTarget* RTarget = nullptr;

		uint32_t bindingCount = 0;

	};

}