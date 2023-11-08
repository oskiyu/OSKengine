#pragma once

#include "OSKmacros.h"

#include "UniquePtr.hpp"
#include "OwnedPtr.h"

#include "MaterialInstance.h"
#include "IBottomLevelAccelerationStructure.h"
#include "Animator.h"

#include <unordered_map>

namespace OSK::GRAPHICS {

	/// @brief Información por cada mesh 3D de un modelo 3D.
	class OSKAPI_CALL PerMeshData {

	public:

		/// @brief Establece la instancia que se usará para el renderizado del mesh.
		/// @param instance Instancia de renderizado.
		void _SetMaterialInstance(OwnedPtr<MaterialInstance> instance);

		/// @brief Establece la estructura de aceleración de bajo nivel que usará
		/// el mesh.
		/// @param blas Estructura de aceleración de bajo nivel.
		void _SetBlas(OwnedPtr<IBottomLevelAccelerationStructure> blas);

		/// @return Instancia de material que debe usar este mesh.
		MaterialInstance* GetMaterialInstance();
		/// @return Instancia de material que debe usar este mesh.
		const MaterialInstance* GetMaterialInstance() const;

	private:

		UniquePtr<MaterialInstance> m_materialInstance = nullptr;
		UniquePtr<IBottomLevelAccelerationStructure> m_bottomLevelAS = nullptr;

	};


	/// @brief Información por cada modelo 3D.
	class OSKAPI_CALL PerModelData {

	public:

		void RegisterMesh(UIndex64 meshId);
		void UnregisterMesh(UIndex64 meshId);

		PerMeshData& GetMeshData(UIndex64 meshId);
		const PerMeshData& GetMeshData(UIndex64 meshId) const;

		bool HasMesh(UIndex64 meshId) const;

	private:

		/// @brief Información de los meshes del modelo.
		std::unordered_map<UIndex64, PerMeshData> m_meshesPerModel;

	};


	/// @brief Estructura de datos con la información relevante de
	/// cada modelo y cada mesh para un pase de renderizado.
	class OSKAPI_CALL MeshMapping {

	public:

		void RegisterModel(UIndex64 modelId);
		void UnregisterModel(UIndex64 modelId);

		/// @param modelId ID del modelo.
		/// @return Datos del modelo.
		PerModelData& GetModelData(UIndex64 modelId);

		/// @param modelId ID del modelo.
		/// @return Datos del modelo.
		const PerModelData& GetModelData(UIndex64 modelId) const;


		/// @return True si tiene registrado un modelo con el ID @p modelId.
		bool HasModel(UIndex64 modelId) const;

	private:

		/// @brief Datos por cada modelo.
		std::unordered_map<UIndex64, PerModelData> m_meshesPerModel;

	};

}
