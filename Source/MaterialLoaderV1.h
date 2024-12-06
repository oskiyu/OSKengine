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
	/// @param materialInfo Información del material.
	/// @param info Información del pipeline (será modificado).
	/// @param type Tipo de material.
	/// @return Layout del material.
	/// 
	/// @throws InvalidDescriptionFileException si la información del 
	/// material no es correcta.
	OwnedPtr<MaterialLayout> LoadMaterialLayoutV1(
		const nlohmann::json& materialInfo, 
		PipelineCreateInfo* info, 
		MaterialType type);


	/// @brief Genera el layout de un material de rasterizado.
	/// @param materialInfo Información del material.
	/// @param info Información del pipeline (será modificado).
	/// @param type Tipo de material.
	/// @return Layout del material.
	/// 
	/// @throws InvalidDescriptionFileException si la información del 
	/// material no es correcta.
	static OwnedPtr<MaterialLayout> LoadGraphicsMaterialLayoutV1(
		const nlohmann::json& materialInfo,
		PipelineCreateInfo* info);

	/// @brief Genera el layout de un material de computación.
	/// @param materialInfo Información del material.
	/// @param info Información del pipeline (será modificado).
	/// @param type Tipo de material.
	/// @return Layout del material.
	/// 
	/// @throws InvalidDescriptionFileException si la información del 
	/// material no es correcta.
	static OwnedPtr<MaterialLayout> LoadComputeMaterialLayoutV1(
		const nlohmann::json& materialInfo,
		PipelineCreateInfo* info);

	/// @brief Genera el layout de un material de meshes.
	/// @param materialInfo Información del material.
	/// @param info Información del pipeline (será modificado).
	/// @param type Tipo de material.
	/// @return Layout del material.
	/// 
	/// @throws InvalidDescriptionFileException si la información del 
	/// material no es correcta.
	static OwnedPtr<MaterialLayout> LoadMeshMaterialLayoutV1(
		const nlohmann::json& materialInfo,
		PipelineCreateInfo* info);

	/// @brief Genera el layout de un material de trazado de rayos.
	/// @param materialInfo Información del material.
	/// @param info Información del pipeline (será modificado).
	/// @param type Tipo de material.
	/// @return Layout del material.
	/// 
	/// @throws InvalidDescriptionFileException si la información del 
	/// material no es correcta.
	static OwnedPtr<MaterialLayout> LoadRayTracingMaterialLayoutV1(
		const nlohmann::json& materialInfo,
		PipelineCreateInfo* info);


	/// @param materialInfo Información del material.
	/// @return Mapa índice del slot -> nombre del slot.
	static std::unordered_map<UIndex32, std::string> GetSlotsNames(const nlohmann::json& materialInfo);

}
