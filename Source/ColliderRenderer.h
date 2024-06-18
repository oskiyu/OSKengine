// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#pragma once

#include "IRenderSystem.h"
#include "ResourcesInFlight.h"
#include "GpuBuffer.h"
#include "HashMap.hpp"
#include "AssetRef.h"
#include "UniquePtr.hpp"
#include "ISystem.h"
#include "MaterialInstance.h"
#include <array>
#include <unordered_map>
#include "DynamicArray.hpp"

namespace OSK::ASSETS {
	class Model3D;
}
namespace OSK::COLLISION {
	class IBottomLevelCollider;
}

namespace OSK::ECS {

	/// @brief Sistema de renderizado que renderiza los colliders
	/// de todas las entidades.
	/// 
	/// @note Para que los bottom level colliders se renderizen correctamente,
	/// cada entidad debe registrarse en el sistema mediante SetupBottomLevelModel.
	/// En caso contrario, sólamente se renderizará su collider de alto nivel.
	class OSKAPI_CALL ColliderRenderSystem : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::ColliderRenderSystem");

		ColliderRenderSystem();

		/// @brief Establece la cámara de renderizado.
		/// @param cameraObject Objeto con la cámara.
		/// 
		/// @pre La entidad debe contener un componente de cámara 3D.
		void Initialize(GameObjectIndex cameraObject);

		void CreateTargetImage(const Vector2ui& size) override;
		void Render(GRAPHICS::ICommandList* commandList, std::span<const ECS::GameObjectIndex> objects) override;

		void OnObjectAdded(GameObjectIndex obj) override;
		void OnObjectRemoved(GameObjectIndex obj) override;

		/// @brief Registra la entidad para que se renderizen sus
		/// colliders de bajo nivel.
		/// @param obj Entidad.
		/// 
		/// @pre La entidad debe tener un componente de colisión.
		void SetupBottomLevelModel(GameObjectIndex obj);

		nlohmann::json SaveConfiguration() const override;
		PERSISTENCE::BinaryBlock SaveBinaryConfiguration() const override;
		void ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) override;
		void ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) override;

	private:

		GameObjectIndex m_cameraObject = EMPTY_GAME_OBJECT;
		std::array<UniquePtr<GRAPHICS::GpuBuffer>, GRAPHICS::MAX_RESOURCES_IN_FLIGHT> m_cameraGpuBuffers;

		/// @brief Material usado para el renderizado de colliders
		/// para los que se tiene un modelo 3D (cubo o esfera).
		GRAPHICS::Material* material = nullptr;

		/// @brief Material usado para el renderizado de colliders
		/// para los que NO se tiene un modelo 3D (SAT).
		/// Usa líneas en vez de triángulos.
		GRAPHICS::Material* lowLevelMaterial = nullptr;

		/// @brief Material usado para el renderizado de puntos
		/// de colisión.
		GRAPHICS::Material* pointMaterial = nullptr;


		/// @brief Instancia válida para cualquiera de los dos materiales (menos
		/// el de puntos de colisión).
		/// Una por cada recurso en vuelo, ya que también hay un
		/// buffer de cámara por cada recurso en vuelo.
		std::array<UniquePtr<GRAPHICS::MaterialInstance>, GRAPHICS::MAX_RESOURCES_IN_FLIGHT> m_materialInstances;

		/// @brief Instancia para el renderizado de puntos.
		/// Una por cada recurso en vuelo, ya que también hay un
		/// buffer de cámara por cada recurso en vuelo.
		std::array<UniquePtr<GRAPHICS::MaterialInstance>, GRAPHICS::MAX_RESOURCES_IN_FLIGHT> m_pointsMaterialInstances;


		/// @brief Contiene los buffers de vértices de los colliders que 
		/// NO tienen modelo 3D (SAT).
		/// Cada buffer representa una cara.
		std::unordered_map<GameObjectIndex, DynamicArray<UniquePtr<GRAPHICS::GpuBuffer>>> m_bottomLevelVertexBuffers;

		/// @brief Contiene los buffers de índices de los colliders que 
		/// NO tienen modelo 3D (SAT).
		/// Cada buffer representa una cara.
		std::unordered_map<GameObjectIndex, DynamicArray<UniquePtr<GRAPHICS::GpuBuffer>>> m_bottomLevelIndexBuffers;


		ASSETS::AssetRef<ASSETS::Model3D> m_cubeModel;
		ASSETS::AssetRef<ASSETS::Model3D> m_sphereModel;

	};

}
