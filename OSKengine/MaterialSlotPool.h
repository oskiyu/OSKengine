#pragma once

#include "DescriptorSet.h"
#include "DescriptorLayout.h"

#include "MaterialSlotType.h"
#include "OwnedPtr.h"
#include <stack>
#include <vector>

namespace OSK {

	class Material;
	class MaterialSlot;
	class MaterialSlotData;
	class RenderAPI;
	class MaterialSystem;

	/// <summary>
	/// Almacena un conjunto de slots.
	/// </summary>
	class OSKAPI_CALL MaterialSlotPool {

		friend class MaterialInstance;
		friend class MaterialSlot;

	public:

		/// <summary>
		/// Crea el pool.
		/// </summary>
		/// <param name="renderer">Renderizador.</param>
		/// <param name="type">Tipo de slot.</param>
		/// <param name="system">Sistema de materiales del renderizador.</param>
		MaterialSlotPool(RenderAPI* renderer, MaterialSlotTypeId type, MaterialSystem* system, Material* material);
		~MaterialSlotPool();

		/// <summary>
		/// Devuelve el identificador del siguiente slot libre.
		/// </summary>
		uint32_t GetNextMaterialSlot();
		/// <summary>
		/// Devuelve el material slot dado su identificador.
		/// </summary>
		MaterialSlotData* GetMaterialSlot(uint32_t id);

		/// <summary>
		/// True si no hay más huecos libres.
		/// </summary>
		bool IsFull() const;
		/// <summary>
		/// True si no hay ningún slot ocupado.
		/// </summary>
		bool IsEmpty() const;

		/// <summary>
		/// Número de slots en cada pool.
		/// </summary>
		const TSize PoolSize = 256;

		DescriptorSet* GetDescriptorSet(uint32_t id);

	private:

		void FreeSlot(uint32_t id);

		OwnedPtr<DescriptorPool> pool = nullptr;

		MaterialSlotTypeId type;
		std::stack<uint32_t> freeSpaces;

		std::vector<OwnedPtr<MaterialSlotData>> materialSlots;
		std::vector<OwnedPtr<DescriptorSet>> descriptorSets;
		RenderAPI* renderer = nullptr;

	};

}
