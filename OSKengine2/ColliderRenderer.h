// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#pragma once

#include "IRenderSystem.h"

#include "IGpuUniformBuffer.h"

#include "HashMap.hpp"
#include "IGpuVertexBuffer.h"
#include "IGpuIndexBuffer.h"

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

		/// @brief Registra la entidad para que se renderizen sus
		/// colliders de bajo nivel.
		/// @param obj Entidad.
		/// 
		/// @pre La entidad debe tener un componente de colisión.
		void SetupBottomLevelModel(GameObjectIndex obj);

	private:

		GameObjectIndex cameraObject = EMPTY_GAME_OBJECT;
		UniquePtr<GRAPHICS::IGpuUniformBuffer> cameraUbos[NUM_RESOURCES_IN_FLIGHT]{};

		/// @brief Material usado para el renderizado de colliders
		/// para los que se tiene un modelo 3D (cubo o esfera).
		GRAPHICS::Material* material = nullptr;

		/// @brief Material usado para el renderizado de colliders
		/// para los que NO se tiene un modelo 3D (SAT).
		/// Usa líneas en vez de triángulos.
		GRAPHICS::Material* lowLevelMaterial = nullptr;

		/// @brief Instancia válida para cualquiera de los dos materiales.
		UniquePtr<GRAPHICS::MaterialInstance> materialInstance;

		/// @brief Contiene los buffers de vértices de los colliders que 
		/// NO tienen modelo 3D (SAT).
		/// Cada buffer representa una cara.
		HashMap<GameObjectIndex, DynamicArray<UniquePtr<GRAPHICS::IGpuVertexBuffer>>> bottomLevelVertexBuffers;

		/// @brief Contiene los buffers de índices de los colliders que 
		/// NO tienen modelo 3D (SAT).
		/// Cada buffer representa una cara.
		HashMap<GameObjectIndex, DynamicArray<UniquePtr<GRAPHICS::IGpuIndexBuffer>>> bottomLevelIndexBuffers;


		const ASSETS::Model3D* cubeModel = nullptr;
		const ASSETS::Model3D* sphereModel = nullptr;

	};

}
