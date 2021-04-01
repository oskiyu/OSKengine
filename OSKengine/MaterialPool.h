#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "DescriptorLayout.h"
#include "DescriptorSet.h"
#include "MaterialBinding.h"
#include "MaterialInstance.h"

#include "ArrayStack.h"
#include "MaterialPoolData.h"

namespace OSK {

	class RenderAPI;
	class Material;

	//Pool de un material. Almacena los descriptor pools.
	class OSKAPI_CALL MaterialPool {

		friend class Material;

	public:

		MaterialPool(RenderAPI* renderer) : Renderer(renderer) {
			
		}

		~MaterialPool();

		//Añade un binding al layout.
		void AddBinding(MaterialBindingType binding, MaterialBindingShaderStage shaderStage);

		inline void SetBinding(const MaterialBindingLayout& layout) {
			Bindings = layout;
		}

		//Crea y devuelve una nueva instancia del material.
		MaterialInstance* CreateInstance();

		//Libera todos los descriptor pools.
		//No resetea el layout.
		void Free();

		DescriptorSet* GetDSet(uint32_t index) {
			uint32_t bucket = index / MaterialPoolSize;
			uint32_t offset = index % MaterialPoolSize;

			return Datas[bucket].DescriptorSets[offset];
		}

		void FreeSet(uint32_t index) {
			uint32_t bucket = index / MaterialPoolSize;
			
			Datas[bucket].FreeSpaces.Push(index);
		}

	private:

		RenderAPI* Renderer = nullptr;
		DescriptorLayout* Layout = nullptr;

		void AddNewData();

		/*LAYOUT*/
		MaterialBindingLayout Bindings = {};
		std::vector<MaterialPoolData> Datas = {};

	};

}
