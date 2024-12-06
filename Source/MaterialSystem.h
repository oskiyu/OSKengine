#pragma once

#include "ApiCall.h"

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

		MaterialSystem() = default;
		~MaterialSystem();

		OSK_DISABLE_COPY(MaterialSystem);

		/// @brief Carga un material.
		/// 
		/// Los materiales son cacheados: sólo se cargan una vez.
		/// @param path Ruta al archivo del material (.json).
		/// @return Material definido por el archivo.
		/// 
		/// @throws ASSETS::InvalidDescriptionFileException Si el archivo del material tiene información
		/// faltante o incorrecta.
		Material* LoadMaterial(const std::string& path);

		/// @brief Devuelve el material cargado con el nombre dado.
		/// @param name Nombre del material.
		/// @return Material.
		/// 
		/// @pre El material debe haber sido previamente cargado con LoadMaterial.
		/// @throws MaterialNotFoundException si el material no ha sido previamente cargado.
		Material* GetMaterialByName(const std::string& name) const;


		/// @brief Registra un nuevo tipo de vértice, para que puedan crearse 
		/// materiales para él.
		/// @tparam T Clase del tipo de vértice.
		/// 
		/// @pre La clase 'T' debe tener implementado OSK_REG_VERTEX_TYPE de manera correcta.
		template <typename T> void RegisterVertexType() {
			VertexInfo info = T::GetVertexInfo();
			const std::string name = static_cast<std::string>(T::GetVertexTypeName());
			vertexTypesTable[name] = std::move(info);
		}


		/// @brief Recarga el material indicado.
		/// @param path Ruta del material.
		/// 
		/// @pre El material indicado debe haber sido previamente cargado.
		/// @throws MaterialNotFoundException si el material no ha sido previamente cargado.
		void ReloadMaterialByPath(const std::string& path);

		/// @brief Recarga el material indicado. 
		/// @param name Nombre del material.
		/// 
		/// @pre El material indicado debe haber sido previamente cargado.
		/// @throws MaterialNotFoundException si el material no ha sido previamente cargado.
		void ReloadMaterialByName(const std::string& name);

		/// @brief Recarga todos los materiales.
		void ReloadAllMaterials();

	private:

		/// @throws ASSETS::InvalidDescriptionFileException Si el archivo de material es incorrecto.
		void LoadMaterialV0(MaterialLayout* layout, const nlohmann::json& materialInfo, PipelineCreateInfo* info);

		DynamicArray<UniquePtr<Material>> materials;
		std::unordered_map<std::string, Material*, StringHasher, std::equal_to<>> materialsPathTable;
		std::unordered_map<std::string, Material*, StringHasher, std::equal_to<>> materialsNameTable;
		std::unordered_map<std::string, VertexInfo, StringHasher, std::equal_to<>> vertexTypesTable;
		DynamicArray<const IRenderpass*> registeredRenderpasses;

	};

}
