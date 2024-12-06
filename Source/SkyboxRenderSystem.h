#pragma once

#include "ApiCall.h"
#include "UniquePtr.hpp"
#include "Vector2.hpp"

#include "GameObject.h"
#include "ISystem.h"
#include "IRenderSystem.h"

#include "MaterialInstance.h"
#include "GpuBuffer.h"
#include "ResourcesInFlight.h"

#include "CubemapTexture.h"
#include "Model3D.h"
#include "AssetRef.h"

#include "SavedGameObjectTranslator.h"
#include "BinaryBlock.h"

#include <span>
#include <array>
#include <json.hpp>


namespace OSK::GRAPHICS {
	class ICommandList;
	class Material;
}

namespace OSK::ECS {

	class OSKAPI_CALL SkyboxRenderSystem : public IRenderSystem, public ISerializableSystem {

	public:

		constexpr static auto MaterialName = "Resources/Materials/Skybox/material.json";

		OSK_SYSTEM("OSK::SkyboxRenderSystem");

		SkyboxRenderSystem();

		void CreateTargetImage(const Vector2ui& size) override;

		void SetCamera(GameObjectIndex cameraObject);
		void SetCubemap(ASSETS::AssetRef<ASSETS::CubemapTexture> texture);

		void Render(GRAPHICS::ICommandList* commandList, std::span<const ECS::GameObjectIndex> objects) override;

		nlohmann::json SaveConfiguration() const override;
		PERSISTENCE::BinaryBlock SaveBinaryConfiguration() const override;
		void ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) override;
		void ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) override;

		const ASSETS::CubemapTexture* GetCurrentTexture() const;
		GameObjectIndex GetCurrentCameraObject() const;
		const GRAPHICS::Material* GetCurrentMaterial() const;

	private:

		constexpr static auto CubeModelPath = "Resources/Assets/Models/cube.json";

		GRAPHICS::Material* m_skyboxMaterial = nullptr;

		std::array<UniquePtr<GRAPHICS::MaterialInstance>, GRAPHICS::MAX_RESOURCES_IN_FLIGHT> m_skyboxMaterialInstances;
		std::array<UniquePtr<GRAPHICS::GpuBuffer>, GRAPHICS::MAX_RESOURCES_IN_FLIGHT> m_cameraUbos{};

		GameObjectIndex cameraObject = EMPTY_GAME_OBJECT;

		ASSETS::AssetRef<ASSETS::CubemapTexture> m_skybox;
		ASSETS::AssetRef<ASSETS::Model3D> m_cubemapModel;

	};

}
