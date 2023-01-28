#pragma once

#include "OSKmacros.h"

#include "LinkedList.hpp"
#include "DynamicArray.hpp"
#include "HashMap.hpp"
#include "OwnedPtr.h"
#include "UniquePtr.hpp"
#include "VertexInfo.h"

#include <string>
#include <json.hpp>

namespace OSK::GRAPHICS {

	class Material;
	class IRenderpass;
	class VertexInfo;
	class MaterialLayout;
	struct PipelineCreateInfo;
	enum class MaterialType;


	/// @brief Clase que se encarga de cargar y manejar los materiales.
	/// Un material define el comportamiento del renderizador con un objeto en concreto.
	class OSKAPI_CALL MaterialSystem {

	public:

		~MaterialSystem();

		/// @brief Carga un material.
		/// 
		/// Los materiales son cacheados: sólo se cargan una vez.
		/// @param path Ruta al archivo del material (.json).
		/// @return Material definido por el archivo.
		Material* LoadMaterial(const std::string& path);

		/// @brief Registra un nuevo tipo de vértice, para que puedan crearse 
		/// materiales para él.
		/// @tparam T Clase del tipo de vértice.
		/// 
		/// @pre La clase 'T' debe tener implementado OSK_REG_VERTEX_TYPE de manera correcta.
		template <typename T> void RegisterVertexType() {
			vertexTypesTable.Insert(T::GetVertexTypeName(), T::GetVertexInfo());
		}

	private:

		void LoadMaterialV0(MaterialLayout* layout, const nlohmann::json& materialInfo, PipelineCreateInfo* info);
		void LoadMaterialV1(MaterialLayout* layout, const nlohmann::json& materialInfo, PipelineCreateInfo* info, MaterialType type);

		DynamicArray<UniquePtr<Material>> materials;
		HashMap<std::string, Material*> materialsTable;
		HashMap<std::string, VertexInfo> vertexTypesTable;
		DynamicArray<const IRenderpass*> registeredRenderpasses;

	};

}
