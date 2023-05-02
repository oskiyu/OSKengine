// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#pragma once

#include "IRenderSystem.h"
#include "GpuBuffer.h"
#include "HashMap.hpp"

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
		void Render(GRAPHICS::ICommandList* commandList) override;

		void OnObjectAdded(GameObjectIndex obj) override;
		void OnObjectRemoved(GameObjectIndex obj) override;

		/// @brief Registra la entidad para que se renderizen sus
		/// colliders de bajo nivel.
		/// @param obj Entidad.
		/// 
		/// @pre La entidad debe tener un componente de colisión.
		void SetupBottomLevelModel(GameObjectIndex obj);

	private:

		GameObjectIndex cameraObject = EMPTY_GAME_OBJECT;
		UniquePtr<GRAPHICS::GpuBuffer> cameraUbos[NUM_RESOURCES_IN_FLIGHT]{};

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
		UniquePtr<GRAPHICS::MaterialInstance> materialInstance;

		/// @brief Instancia para el renderizado de puntos.
		UniquePtr<GRAPHICS::MaterialInstance> pointsMaterialInstance;


		/// @brief Contiene los buffers de vértices de los colliders que 
		/// NO tienen modelo 3D (SAT).
		/// Cada buffer representa una cara.
		HashMap<GameObjectIndex, DynamicArray<UniquePtr<GRAPHICS::GpuBuffer>>> bottomLevelVertexBuffers;

		/// @brief Contiene los buffers de índices de los colliders que 
		/// NO tienen modelo 3D (SAT).
		/// Cada buffer representa una cara.
		HashMap<GameObjectIndex, DynamicArray<UniquePtr<GRAPHICS::GpuBuffer>>> bottomLevelIndexBuffers;


		const ASSETS::Model3D* cubeModel = nullptr;
		const ASSETS::Model3D* sphereModel = nullptr;

	};

}
