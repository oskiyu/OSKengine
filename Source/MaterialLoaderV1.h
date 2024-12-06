#pragma once

#include <json.hpp>
#include "OwnedPtr.h"
#include "MaterialLayout.h"
#include "NumericTypes.h"
#include "HashMap.hpp"

namespace OSK::GRAPHICS {

	struct PipelineCreateInfo;
	enum class MaterialType;


	/// @brief Genera el layout de un material.
	/// @param materialInfo Informaci�n del material.
	/// @param info Informaci�n del pipeline (ser� modificado).
	/// @param type Tipo de material.
	/// @return Layout del material.
	/// 
	/// @throws InvalidDescriptionFileException si la informaci�n del 
	/// material no es correcta.
	OwnedPtr<MaterialLayout> LoadMaterialLayoutV1(
		const nlohmann::json& materialInfo, 
		PipelineCreateInfo* info, 
		MaterialType type);


	/// @brief Genera el layout de un material de rasterizado.
	/// @param materialInfo Informaci�n del material.
	/// @param info Informaci�n del pipeline (ser� modificado).
	/// @param type Tipo de material.
	/// @return Layout del material.
	/// 
	/// @throws InvalidDescriptionFileException si la informaci�n del 
	/// material no es correcta.
	static OwnedPtr<MaterialLayout> LoadGraphicsMaterialLayoutV1(
		const nlohmann::json& materialInfo,
		PipelineCreateInfo* info);

	/// @brief Genera el layout de un material de computaci�n.
	/// @param materialInfo Informaci�n del material.
	/// @param info Informaci�n del pipeline (ser� modificado).
	/// @param type Tipo de material.
	/// @return Layout del material.
	/// 
	/// @throws InvalidDescriptionFileException si la informaci�n del 
	/// material no es correcta.
	static OwnedPtr<MaterialLayout> LoadComputeMaterialLayoutV1(
		const nlohmann::json& materialInfo,
		PipelineCreateInfo* info);

	/// @brief Genera el layout de un material de meshes.
	/// @param materialInfo Informaci�n del material.
	/// @param info Informaci�n del pipeline (ser� modificado).
	/// @param type Tipo de material.
	/// @return Layout del material.
	/// 
	/// @throws InvalidDescriptionFileException si la informaci�n del 
	/// material no es correcta.
	static OwnedPtr<MaterialLayout> LoadMeshMaterialLayoutV1(
		const nlohmann::json& materialInfo,
		PipelineCreateInfo* info);

	/// @brief Genera el layout de un material de trazado de rayos.
	/// @param materialInfo Informaci�n del material.
	/// @param info Informaci�n del pipeline (ser� modificado).
	/// @param type Tipo de material.
	/// @return Layout del material.
	/// 
	/// @throws InvalidDescriptionFileException si la informaci�n del 
	/// material no es correcta.
	static OwnedPtr<MaterialLayout> LoadRayTracingMaterialLayoutV1(
		const nlohmann::json& materialInfo,
		PipelineCreateInfo* info);


	/// @param materialInfo Informaci�n del material.
	/// @return Mapa �ndice del slot -> nombre del slot.
	static std::unordered_map<UIndex32, std::string> GetSlotsNames(const nlohmann::json& materialInfo);

}
