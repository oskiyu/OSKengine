#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "DescriptorLayout.h"
#include "MaterialBinding.h"
#include "MaterialInstance.h"

namespace OSK {

	class RenderAPI;

	typedef	std::vector<MaterialBinding> MaterialBindingLayout;

	//Contiene los vulkan descriptor pools con un tamaño de <MaxSize>.
	struct OSKAPI_CALL MaterialPoolData {
		//Layout (pool).
		DescriptorLayout* Layout = nullptr;
		//Número máximo de elementos.
		const uint32_t MaxSize = 32;
		//Número actual de elementos en el pool.
		uint32_t CurrentSize = 0;

		//Libera el pool.
		inline void Free() {
			SafeDelete(&Layout);
			CurrentSize = 0;
		}

		//True si el pool está lleno.
		inline bool IsFull() const {
			return CurrentSize == MaxSize - 1;
		}

	};


	//Pool de un material. Almacena los descriptor pools.
	class OSKAPI_CALL MaterialPool {

	public:

		~MaterialPool() {
			Free();
		}

		//Añade un binding al layout.
		inline void AddBinding(MaterialBindingType binding, MaterialBindingShaderStage shaderStage) {
			Bindings.push_back({ binding, shaderStage });
		}

		inline void SetBinding(const MaterialBindingLayout& layout) {
			Bindings = layout;
		}

		//Crea y devuelve una nueva instancia del material.
		MaterialInstance CreateInstance();

		//Libera todos los descriptor pools.
		//No resetea el layout.
		void Free();

	private:

		MaterialPool(RenderAPI* renderer) : Renderer(renderer) {

		}

		RenderAPI* Renderer = nullptr;

		void AddNewData();

		/*LAYOUT*/
		MaterialBindingLayout Bindings = {};
		std::vector<MaterialPoolData> Datas = {};

		/*TO VULKAN*/
		VkDescriptorType GetVulkanBindingType(MaterialBindingType type) const;
		VkShaderStageFlags GetVulkanShaderBinding(MaterialBindingShaderStage type) const;

	};

}
