#pragma once

#include <json.hpp>
#include "UniquePtr.hpp"
#include "MaterialLayout.h"
#include "NumericTypes.h"
#include "HashMap.hpp"
#include "LoadedMaterialInfo.h"

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
	UniquePtr<MaterialLayout> LoadMaterialLayoutV1(
		const LoadedMaterialInfoV1& materialInfo,
		PipelineCreateInfo* info, 
		MaterialType type);


	// Lee el JSON con la información del material.
	LoadedMaterialInfoV1 GetMaterialInfoV1(const nlohmann::json& materialInfo);


	/// @brief Genera el layout de un material de rasterizado.
	/// @param materialInfo Información del material.
	/// @param info Información del pipeline (será modificado).
	/// @param type Tipo de material.
	/// @return Layout del material.
	/// 
	/// @throws InvalidDescriptionFileException si la información del 
	/// material no es correcta.
	static UniquePtr<MaterialLayout> LoadGraphicsMaterialLayoutV1(
		const LoadedMaterialInfoV1& materialInfo,
		PipelineCreateInfo* info);

	/// @brief Genera el layout de un material de computación.
	/// @param materialInfo Información del material.
	/// @param info Información del pipeline (será modificado).
	/// @param type Tipo de material.
	/// @return Layout del material.
	/// 
	/// @throws InvalidDescriptionFileException si la información del 
	/// material no es correcta.
	static UniquePtr<MaterialLayout> LoadComputeMaterialLayoutV1(
		const LoadedMaterialInfoV1& materialInfo,
		PipelineCreateInfo* info);

	/// @brief Genera el layout de un material de meshes.
	/// @param materialInfo Información del material.
	/// @param info Información del pipeline (será modificado).
	/// @param type Tipo de material.
	/// @return Layout del material.
	/// 
	/// @throws InvalidDescriptionFileException si la información del 
	/// material no es correcta.
	static UniquePtr<MaterialLayout> LoadMeshMaterialLayoutV1(
		const LoadedMaterialInfoV1& materialInfo,
		PipelineCreateInfo* info);

	/// @brief Genera el layout de un material de trazado de rayos.
	/// @param materialInfo Información del material.
	/// @param info Información del pipeline (será modificado).
	/// @param type Tipo de material.
	/// @return Layout del material.
	/// 
	/// @throws InvalidDescriptionFileException si la información del 
	/// material no es correcta.
	static UniquePtr<MaterialLayout> LoadRayTracingMaterialLayoutV1(
		const LoadedMaterialInfoV1& materialInfo,
		PipelineCreateInfo* info);


	/// @param materialInfo Información del material.
	/// @return Mapa índice del slot -> nombre del slot.
	static std::unordered_map<UIndex32, std::string> GetSlotsNames(const nlohmann::json& materialInfo);

}
