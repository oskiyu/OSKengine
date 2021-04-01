#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "MaterialPool.h"
#include "MaterialInstance.h"
#include "MaterialPipelineInfo.h"

#include "GraphicsPipeline.h"
#include "Renderpass.h"

#include <unordered_map>
#include <map>
#include <string>

namespace OSK {

	class RenderAPI;
	class MaterialSystem;

	class OSKAPI_CALL Material {

		friend class MaterialSystem;

	public:

		~Material();

		void SetRenderer(RenderAPI* renderer) {
			Renderer = renderer;
			Pool = new MaterialPool(renderer);
		}

		void SetPipelineSettings(const MaterialPipelineCreateInfo& info);

		void SetLayout(MaterialBindingLayout layout);

		MaterialInstance* CreateInstance();

		uint32_t GetBindingIndex(const std::string& name) {
			return BindingNameToBinding[name];
		}

		void RegisterRenderpass(VULKAN::Renderpass* renderpass);
		void UnregisterRenderpass(VULKAN::Renderpass* renderpass);

		GraphicsPipeline* GetGraphicsPipeline(VULKAN::Renderpass* renderpass) const;

		MaterialPipelineCreateInfo GetMaterialGraphicsPipelineInfo() const;

		static Texture* DefaultTexture;

	private:

		MaterialPipelineCreateInfo PipelineInfo;

		std::unordered_map<std::string, uint32_t> BindingNameToBinding{};
		std::map<VULKAN::Renderpass*, GraphicsPipeline*> Pipelines;

		std::vector<VULKAN::Renderpass*> RenderpassesToRegister;

		MaterialPool* Pool = nullptr;

		DescriptorLayout* MLayout = nullptr;
		RenderAPI* Renderer = nullptr;

	};

}
