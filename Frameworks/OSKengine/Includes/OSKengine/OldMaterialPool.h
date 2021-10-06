#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "DescriptorLayout.h"
#include "DescriptorSet.h"
#include "MaterialBinding.h"
#include "OldMaterialInstance.h"

#include "Stack.hpp"
#include "OldMaterialPoolData.h"

namespace OSK {

	class RenderAPI;
	class OldMaterial;

	/// <summary>
	/// Pool de un material.
	/// Se encarga de crear material instances y de almacenar sus descriptor sets.
	/// </summary>
	class OSKAPI_CALL OldMaterialPool {

		friend class OldMaterial;

	public:

		/// <summary>
		/// Crea el material pool.
		/// </summary>
		/// <param name="renderer">Renderizador.</param>
		OldMaterialPool(const RenderAPI* renderer);

		/// <summary>
		/// Destruye el material pool.
		/// </summary>
		~OldMaterialPool();

		/// <summary>
		/// A�ade un binding al layout.
		/// </summary>
		/// <param name="binding">Tipo de binding.</param>
		/// <param name="shaderStage">Stage del binding.</param>
		void AddBinding(MaterialBindingType binding, MaterialBindingShaderStage shaderStage);

		/// <summary>
		/// Establece el layout del material.
		/// </summary>
		/// <param name="layout">Layout.</param>
		void SetLayout(const MaterialBindingLayout& layout);

		/// <summary>
		/// Crea y devuelve una nueva instancia del material.
		/// </summary>
		/// <returns>Nueva instancia.</returns>
		OldMaterialInstance* CreateInstance();

		/// <summary>
		/// Libera todos los descriptor pools.
		/// No resetea el layout.
		/// </summary>
		void Free();

		/// <summary>
		/// Obtiene el descriptor set de un material instance.
		/// </summary>
		/// <param name="index">ID del material instance.</param>
		/// <returns>Descriptor set.</returns>
		DescriptorSet* GetDSet(uint32_t index);

		/// <summary>
		/// Libera el descriptor set de un material instance que ya no existe.
		/// </summary>
		/// <param name="index">ID del material instance.</param>
		void FreeSet(uint32_t index);

	private:

		/// <summary>
		/// Renderizador.
		/// </summary>
		const RenderAPI* renderer = nullptr;

		/// <summary>
		/// Layout del material.
		/// </summary>
		DescriptorLayout* layout = nullptr;

		/// <summary>
		/// A�ade un nuevo MaterialPoolData.
		/// </summary>
		void AddNewData();

		/// <summary>
		/// Layout del material.
		/// </summary>
		MaterialBindingLayout bindings = {};

		/// <summary>
		/// MaterialPoolDatas.
		/// </summary>
		std::vector<OldMaterialPoolData*> datas = {};

	};

}
