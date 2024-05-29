#pragma once

#include "ApiCall.h"

#include "UniquePtr.hpp"
#include "OwnedPtr.h"

#include "MaterialInstance.h"
#include "IBottomLevelAccelerationStructure.h"
#include "Animator.h"

#include <unordered_map>

#include "GpuModel3D.h"
#include "GpuMesh3D.h"


namespace OSK::GRAPHICS {


	/// @brief Información por cada modelo 3D.
	class OSKAPI_CALL GlobalPerModelData {

	public:

		GlobalPerModelData() = default;

		OSK_DISABLE_COPY(GlobalPerModelData);

		/// @return Instancia de material con los uniform buffers
		/// de la animación.
		MaterialInstance* GetAnimationMaterialInstance();

		/// @return Instancia de material con los uniform buffers
		/// de la animación.
		const MaterialInstance* GetAnimationMaterialInstance() const;

	private:

		UniquePtr<MaterialInstance> m_animationMaterialInstance = nullptr;
		UniquePtr<GpuBuffer> m_bonesBuffer;

	};


	/// @brief Estructura de datos con la información relevante de
	/// cada modelo y cada mesh para un pase de renderizado.
	class OSKAPI_CALL GlobalMeshMapping {

	public:

		GlobalMeshMapping() = default;

		OSK_DISABLE_COPY(GlobalMeshMapping);


		/// @brief Registra un nuevo modelo, añadiendo una entrada
		/// para él.
		/// @param modelId Identificador único del modelo.
		/// 
		/// @pre No debe existir un modelo con identificador @p modelId
		/// registrado.
		void RegisterModel(GpuModelUuid modelId);

		/// @brief Elimina un modelo.
		/// @param modelId Identificador único del modelo.
		/// 
		/// @pre Debe existir un modelo con identificador @p modelId
		/// registrado.
		void UnregisterModel(GpuModelUuid modelId);


		/// @param modelId ID del modelo.
		/// @return Datos del modelo.
		/// 
		/// @pre Debe existir un modelo con identificador @p modelId
		/// registrado.
		GlobalPerModelData& GetModelData(GpuModelUuid modelId);

		/// @param modelId ID del modelo.
		/// @return Datos del modelo.
		/// 
		/// @pre Debe existir un modelo con identificador @p modelId
		/// registrado.
		const GlobalPerModelData& GetModelData(GpuModelUuid modelId) const;


		/// @return True si tiene registrado un modelo con el ID @p modelId.
		bool HasModel(GpuModelUuid modelId) const;


		/// @brief Establece la matrix previa del objeto @p obj.
		/// @param obj Objeto de la matriz.
		/// @param matrix Matriz del fotograma anterior.
		void SetPreviousModelMatrix(ECS::GameObjectIndex obj, glm::mat4 matrix);

		/// @param obj Objeto a obtener.
		/// @return Matriz en el fotograma anterior.
		/// 
		/// @pre Debe haber una matriz registrada para el objeto @p obj.
		/// @throws InvalidArgumentException si se incumple la precondición.
		glm::mat4 GetPreviousModelMatrix(ECS::GameObjectIndex obj) const;

		/// @brief Elimina la matriz del fotograma anterior del objeto @p obj.
		/// @param obj Objeto.
		/// 
		/// @pre Debe haber una matriz registrada para el objeto @p obj.
		/// @throws InvalidArgumentException si se incumple la precondición.
		void RemovePreviousModelMatrix(ECS::GameObjectIndex obj);

		/// @return True si hay una matriz registrada para el objeto @p obj.
		bool HasPreviousModelMatrix(ECS::GameObjectIndex obj) const;

	private:

		/// @brief Datos por cada modelo.
		std::unordered_map<GpuModelUuid, GlobalPerModelData> m_meshesPerModel;

		/// @brief Matrices de los modelos en el fotograma anterior.
		std::unordered_map<ECS::GameObjectIndex, glm::mat4> m_previousModelMatrices;

	};



	// --- Local --- //


	/// @brief Información por cada mesh 3D de un modelo 3D.
	class OSKAPI_CALL LocalPerMeshData {

	public:

		LocalPerMeshData() = default;

		OSK_DISABLE_COPY(LocalPerMeshData);

		/// @brief Establece la instancia que se usará para el renderizado del mesh.
		/// @param instance Instancia de renderizado.
		void _SetMaterialInstance(OwnedPtr<MaterialInstance> instance);

		/// @brief Establece el buffer que contendrá la información
		/// del material PBR.
		/// @param materialBuffer Buffer.
		void _SetMaterialBuffer(OwnedPtr<GpuBuffer> materialBuffer);


		/// @return Instancia de material que debe usar este mesh.
		MaterialInstance* GetMaterialInstance();

		/// @return Instancia de material que debe usar este mesh.
		const MaterialInstance* GetMaterialInstance() const;


		/// @return Buffer con la información del material PBR.
		GpuBuffer* GetMaterialBuffer();

		/// @return Buffer con la información del material PBR.
		const GpuBuffer* GetMaterialBuffer() const;

	private:

		UniquePtr<MaterialInstance> m_materialInstance = nullptr;
		UniquePtr<GpuBuffer> m_materialBuffer;

	};


	/// @brief Información por cada modelo 3D.
	class OSKAPI_CALL LocalPerModelData {

	public:

		LocalPerModelData() = default;

		OSK_DISABLE_COPY(LocalPerModelData);


		/// @brief Registra una nueva malla, añadiendo una entrada
		/// para ella.
		/// @param meshId Identificador único de la malla.
		/// 
		/// @pre No debe existir una malla con identificador @p meshId
		/// registrada.
		void RegisterMesh(GpuMeshUuid meshId);

		/// @brief Elimina una malla.
		/// @param meshId Identificador único de la malla.
		/// 
		/// @pre Debe existir una malla con identificador @p modelId
		/// registrada.
		void UnregisterMesh(GpuMeshUuid meshId);


		/// @param meshId ID de la malla.
		/// @return Información de la malla.
		LocalPerMeshData& GetMeshData(GpuMeshUuid meshId);

		/// @param meshId ID de la malla.
		/// @return Información de la malla.
		const LocalPerMeshData& GetMeshData(GpuMeshUuid meshId) const;


		/// @return True si ha sido registrada una malla con identificador @p meshId.
		bool HasMesh(GpuMeshUuid meshId) const;

	private:

		/// @brief Información de los meshes del modelo.
		std::unordered_map<GpuMeshUuid, LocalPerMeshData> m_meshesPerModel;

	};

	/// @brief Estructura de datos con la información relevante de
	/// cada modelo y cada mesh para un pase de renderizado.
	class OSKAPI_CALL LocalMeshMapping {

	public:

		LocalMeshMapping() = default;

		OSK_DISABLE_COPY(LocalMeshMapping);


		/// @brief Registra un nuevo modelo, añadiendo una entrada
		/// para él.
		/// @param modelId Identificador único del modelo.
		/// 
		/// @pre No debe existir un modelo con identificador @p modelId
		/// registrado.
		void RegisterModel(GpuModelUuid modelId);

		/// @brief Elimina un modelo.
		/// @param modelId Identificador único del modelo.
		/// 
		/// @pre Debe existir un modelo con identificador @p modelId
		/// registrado.
		void UnregisterModel(GpuModelUuid modelId);


		/// @param modelId ID del modelo.
		/// @return Datos del modelo.
		/// 
		/// @pre Debe existir un modelo con identificador @p modelId
		/// registrado.
		LocalPerModelData& GetModelData(GpuModelUuid modelId);

		/// @param modelId ID del modelo.
		/// @return Datos del modelo.
		/// 
		/// @pre Debe existir un modelo con identificador @p modelId
		/// registrado.
		const LocalPerModelData& GetModelData(GpuModelUuid modelId) const;


		/// @return True si tiene registrado un modelo con el ID @p modelId.
		bool HasModel(GpuModelUuid modelId) const;

	private:

		/// @brief Datos por cada modelo.
		std::unordered_map<GpuModelUuid, LocalPerModelData> m_meshesPerModel;

	};

}
