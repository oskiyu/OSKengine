#pragma once

#include "MaterialSlotType.h"
#include "DescriptorSet.h"

namespace OSK {

	class MaterialSlot;
	class MaterialSlotPool;

	/// <summary>
	/// Instancia de un material. Define los slots que posee un objeto.
	/// </summary>
	class OSKAPI_CALL MaterialInstance {

	public:

		~MaterialInstance();

		/// <summary>
		/// Establece el slot que pooserá para el tipo dado.
		/// </summary>
		void SetMaterialSlot(MaterialSlotTypeId type, MaterialSlotPool* pool);

		/// <summary>
		/// Devuelve el slot dado su tipo.
		/// </summary>
		MaterialSlot* GetMaterialSlot(MaterialSlotTypeId type);

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

	private:

		std::unordered_map<MaterialSlotTypeId, MaterialSlotPool*> slotPools;
		std::unordered_map<MaterialSlotTypeId, uint32_t> slotIds;
		std::vector<MaterialSlotTypeId> types;

		size_t numberOfSlots = 0;

	};

}
