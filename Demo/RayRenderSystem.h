#pragma once

#include <OSKengine/IRenderSystem.h>

#include <OSKengine/UniquePtr.hpp>
#include <OSKengine/GpuBuffer.h>
#include <OSKengine/MaterialInstance.h>

#include <OSKengine/DynamicArray.hpp>
#include <OSKengine/ICommandList.h>


class RayRenderSystem : public OSK::ECS::IRenderSystem {

public:

	OSK_SYSTEM("RayRenderSystem");
	RayRenderSystem();

	void Initialize(
		OSK::ECS::GameObjectIndex cameraObject,
		OSK::ECS::GameObjectIndex carObject);

	void CreateTargetImage(const OSK::Vector2ui& size) override;
	void Render(OSK::GRAPHICS::ICommandList* commandList) override;

	void SetDepthImages(std::span<OSK::GRAPHICS::GpuImage*, 3> images);

private:

	// --- Objects --- //
	OSK::ECS::GameObjectIndex m_cameraObject = OSK::ECS::EMPTY_GAME_OBJECT;
	OSK::ECS::GameObjectIndex m_carObject = OSK::ECS::EMPTY_GAME_OBJECT;


	// --- GPU --- //
	OSK::GRAPHICS::Material* m_pointsMaterial = nullptr;
	OSK::UniquePtr<OSK::GRAPHICS::MaterialInstance> m_pointsMaterialInstance;

	OSK::GRAPHICS::Material* m_linesMaterial = nullptr;
	OSK::UniquePtr<OSK::GRAPHICS::MaterialInstance> m_linesMaterialInstance;

	std::array<OSK::UniquePtr<OSK::GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> m_cameraBuffers;

	OSK::UniquePtr<OSK::GRAPHICS::GpuBuffer> m_pointsVertexBuffer;
	OSK::UniquePtr<OSK::GRAPHICS::GpuBuffer> m_pointsIndexBuffer;

	OSK::DynamicArray<OSK::GRAPHICS::RenderPassImageInfo> m_depthImages;

};
