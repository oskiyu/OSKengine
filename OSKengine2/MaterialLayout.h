#pragma once

#include "MaterialLayoutSlot.h"
#include "OSKmacros.h"
#include "LinkedList.hpp"

namespace OSK {

	/// <summary>
	/// El layout de un material determina qué slots pertenecen a ese material.
	/// </summary>
	class OSKAPI_CALL MaterialLayout {

	public:
		
		/// <summary>
		/// Añade el slot dado al layout.
		/// El nombre del slot se usará como llave.
		/// No pueden haber dos slots con el mismo nombre.
		/// </summary>
		void AddSlot(const MaterialLayoutSlot& slot);

		/// <summary>
		/// Obtiene el slot con el nombre dado, si existe.
		/// UNSAFE: no se hace comprobación de que el slot existe.
		/// </summary>
		MaterialLayoutSlot& GetSlot(const std::string& name) const;

		/// <summary>
		/// Obtiene los nombres de todos los slots registrados en el layout.
		/// </summary>
		const LinkedList<std::string>& GetAllSlotNames() const;

	private:

		std::string name;
		HashMap<std::string, MaterialLayoutSlot> slots;
		LinkedList<std::string> slotNames{};

	};

}
