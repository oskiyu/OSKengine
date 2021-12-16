#pragma once

#include "MaterialSlotType.h"
#include "DescriptorSet.h"

namespace OSK {

	class Material;
	class MaterialSlot;
	class MaterialSlotData;
	class MaterialSlotPool;

	/// <summary>
	/// Instancia de un material. Define los slots que posee un objeto.
	/// </summary>
	class OSKAPI_CALL MaterialInstance {

		friend class Material;

	public:

		~MaterialInstance();

		/// <summary>
		/// Devuelve el slot dado su tipo.
		/// </summary>
		MaterialSlot* GetMaterialSlot(MaterialSlotTypeId type);
		MaterialSlotData* GetMaterialSlotData(MaterialSlotTypeId type);

		/// <summary>
		/// Actualiza todos los slots.
		/// </summary>
		void FlushUpdate();
		/// <summary>
		/// Devuelve el número de slots de la instancia.
		/// </summary>
		size_t GetNumberOfSlots() const;

		/// <summary>
		/// True si todos los slots se han establecido correctamente.
		/// </summary>
		bool HasBeenSet() const;

		Material* GetMaterial() const;

	private:

		void AddType(MaterialSlotTypeId type);

		std::unordered_map<MaterialSlotTypeId, MaterialSlot> slots;
		std::vector<MaterialSlotTypeId> types;

		Material* owner = nullptr;

	};

}
