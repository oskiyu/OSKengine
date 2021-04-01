#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Material.h"

#include <string>
#include <list>

namespace OSK {

	class RenderAPI;

	class OSKAPI_CALL MaterialSystem {

	public:

		MaterialSystem(RenderAPI* renderer);
		~MaterialSystem();

		void RegisterMaterial(const std::string& name);
		Material* GetMaterial(const std::string& name);

		void RegisterRenderpass(VULKAN::Renderpass* renderpass);
		void UnregisterRenderpass(VULKAN::Renderpass* renderpass);

	private:

		std::unordered_map<std::string, Material*> Materials;
		std::list<VULKAN::Renderpass*> Renderpasses;

		RenderAPI* Renderer = nullptr;

	};

}
