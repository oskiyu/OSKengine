#pragma once

#include "OSKmacros.h"

#include "LinkedList.hpp"
#include "HashMap.hpp"
#include "OwnedPtr.h"

#include <string>

namespace OSK {

	class Material;

	/// <summary>
	/// Clase que se encarga de cargar y manejar los materiales.
	/// Un material define el comportamiento del renderizador con un objeto en concreto.
	/// </summary>
	class OSKAPI_CALL MaterialSystem {

	public:

		~MaterialSystem();

		/// <summary>
		/// Carga un material.
		/// </summary>
		/// <param name="path">Ruta al archivo del material (.json).</param>
		Material* LoadMaterial(const std::string& path);

	private:

		LinkedList<OwnedPtr<Material>> materials;
		HashMap<std::string, Material*> materialsTable;

	};

}
