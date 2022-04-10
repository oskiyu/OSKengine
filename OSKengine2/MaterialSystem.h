#pragma once

#include "OSKmacros.h"

#include "LinkedList.hpp"
#include "HashMap.hpp"
#include "OwnedPtr.h"
#include "VertexInfo.h"

#include <string>

namespace OSK::GRAPHICS {

	class Material;
	class IRenderpass;

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

		/// <summary>
		/// Registra el renderpass en todos los materiales.
		/// </summary>
		void RegisterRenderpass(const IRenderpass* renderpass);

		/// <summary>
		/// Quita el renderpass de todos los materiales.
		/// </summary>
		void UnregisterRenderpass(const IRenderpass* renderpass);

		/// <summary>
		/// Registra un nuevo tipo de vértice, para que puedan crearse 
		/// materiales para él.
		/// La clase 'T' debe tener implementado OSK_REG_VERTEX_TYPE de manera correcta.
		/// </summary>
		/// <typeparam name="T">Tipo de vértice.</typeparam>
		template <typename T> void RegisterVertexType() {
			vertexTypesTable.Insert(T::GetVertexTypeName(), T::GetVertexInfo());
		}

	private:

		LinkedList<OwnedPtr<Material>> materials;
		HashMap<std::string, Material*> materialsTable;
		HashMap<std::string, VertexInfo> vertexTypesTable;
		DynamicArray<const IRenderpass*> registeredRenderpasses;

	};

}
