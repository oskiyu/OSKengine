#pragma once

#include "IRenderSystem.h"

#include "UniquePtr.hpp"

#include "OwnedPtr.h"

#include "IShaderPass.h"

#include "MaterialInstance.h"

namespace OSK::ASSETS {
	class IrradianceMap;
	class SpecularMap;
}

namespace OSK::ECS {

	class CameraComponent3D;


	/// @brief 
	class OSKAPI_CALL TreeNormalsRenderSystem : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::TreeNormalsRenderSystem");

		TreeNormalsRenderSystem();


		void Initialize(ECS::GameObjectIndex cameraObject);

		void CreateTargetImage(const Vector2ui& size) override;

		void Render(GRAPHICS::ICommandList* commandList, std::span<const ECS::GameObjectIndex> objects) override;

		void AddShaderPass(OwnedPtr<GRAPHICS::IShaderPass> pass) override;
		
	private:

		/// @brief Estructura con la información almacenada
		/// en los uniform buffers de la cámara.
		struct CameraInfo {
			alignas(16) glm::mat4 projectionMatrix;
			alignas(16) glm::mat4 viewMatrix;
			alignas(16) glm::mat4 projectionViewMatrix;

			alignas(16) Vector4f position;

			alignas(16) Vector2f nearFarPlanes;
		};

	private:

		void CreateBuffers();

	private:

		// -- CÁMARA -- //

		/// @brief Objeto que contiene la cámara desde la que se renderiza la escena.
		ECS::GameObjectIndex m_cameraObject = ECS::EMPTY_GAME_OBJECT;

		/// @brief Buffers CPU->GPU donde se almacena el estado de la cámara en un fotograma.
		std::array<UniquePtr<GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> m_cameraBuffers{};

	};

}
