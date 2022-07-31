#pragma once

#include "OSKmacros.h"

#include "LinkedList.hpp"
#include "HashMap.hpp"
#include "OwnedPtr.h"
#include "VertexInfo.h"

#include <string>
#include <json.hpp>

namespace OSK::GRAPHICS {

	class Material;
	class IRenderpass;
	class VertexInfo;
	class MaterialLayout;
	struct PipelineCreateInfo;

	/// <summary>
	/// Clase que se encarga de cargar y manejar los materiales.
	/// Un material define el comportamiento del renderizador con un objeto en concreto.
	/// </summary>
	class OSKAPI_CALL MaterialSystem {

	public:

		~MaterialSystem();

		/// <summary>
		/// Carga un material.
		/// 
		/// Los materiales son cacheados: sólo se cargan una vez.
		/// </summary>
		/// <param name="path">Ruta al archivo del material (.json).</param>
		Material* LoadMaterial(const std::string& path);

		/// <summary>
		/// Registra un nuevo tipo de vértice, para que puedan crearse 
		/// materiales para él.
		/// 
		/// @warning La clase 'T' debe tener implementado OSK_REG_VERTEX_TYPE de manera correcta.
		/// </summary>
		/// <typeparam name="T">Tipo de vértice.</typeparam>
		template <typename T> void RegisterVertexType() {
			vertexTypesTable.Insert(T::GetVertexTypeName(), T::GetVertexInfo());
		}

	private:

		void LoadMaterialV0(MaterialLayout* layout, const nlohmann::json& materialInfo, PipelineCreateInfo* info);
		void LoadMaterialV1(MaterialLayout* layout, const nlohmann::json& materialInfo, PipelineCreateInfo* info);

		LinkedList<OwnedPtr<Material>> materials;
		HashMap<std::string, Material*> materialsTable;
		HashMap<std::string, VertexInfo> vertexTypesTable;
		DynamicArray<const IRenderpass*> registeredRenderpasses;

	};

}
