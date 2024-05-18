#pragma once

#include "IRenderSystem.h"
#include "UniquePtr.hpp"
#include "GpuBuffer.h"

#include <array>

namespace OSK::ASSETS {
	class Model3D;
}

namespace OSK::ECS {

	class OSKAPI_CALL RenderBoundsRenderer : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::RenderBoundsRenderer");

		RenderBoundsRenderer();

		/// @brief Establece la cámara de renderizado.
		/// @param cameraObject Objeto con la cámara.
		/// 
		/// @pre La entidad debe contener un componente de cámara 3D.
		void Initialize(GameObjectIndex cameraObject);

		void CreateTargetImage(const Vector2ui& size) override;
		void Render(GRAPHICS::ICommandList* commandList, std::span<const ECS::GameObjectIndex> objects) override;

		nlohmann::json SaveConfiguration() const override;
		PERSISTENCE::BinaryBlock SaveBinaryConfiguration() const override;
		void ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) override;
		void ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) override;

	private:

		void LoadMaterials();

		GameObjectIndex m_cameraObject = EMPTY_GAME_OBJECT;
		std::array<UniquePtr<GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> m_cameraUbos;
		
		GRAPHICS::Material* m_material = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> m_materialInstance;

		ASSETS::AssetRef<ASSETS::Model3D> m_sphereModel;

	};

}
