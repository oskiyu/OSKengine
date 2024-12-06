#pragma once

#include "IShaderPass.h"
#include "UniquePtr.hpp"
#include "MaterialInstance.h"
#include "NumericTypes.h"

#include <array>
#include <span>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GrassRenderPass : public IShaderPass {

	public:

		constexpr static auto Name = "GrassRenderPass";

		OSK_RENDERPASS(GrassRenderPass, Name);

		GrassRenderPass() : IShaderPass("GrassRenderPass") {}

		void SetCameraBuffers(std::span<const GRAPHICS::GpuBuffer*, MAX_RESOURCES_IN_FLIGHT> cameraBuffers) {
			for (USize32 i = 0; i < m_cameraBuffers.size(); i++) {
				m_cameraBuffers[i] = cameraBuffers[i];
			}
		}

		void Load() override;

		void Update(TDeltaTime deltaTime) override;
		void RenderLoop(
			ICommandList* commandList,
			const DynamicArray<ECS::GameObjectIndex>& objectsToRender,
			GlobalMeshMapping* meshMapping,
			UIndex32 jitterIndex,
			Vector2ui resolution) override;

	private:

		std::array<const GRAPHICS::GpuBuffer*, MAX_RESOURCES_IN_FLIGHT> m_cameraBuffers{};
		std::array<UniquePtr<MaterialInstance>, MAX_RESOURCES_IN_FLIGHT> m_cameraInstances{};

		struct WindBuffer {
			Vector4f origin{};
		};
		Vector2f m_windOffset = Vector2f::Zero;
		float m_time = 0.0f;

		std::array<UniquePtr<GpuBuffer>, MAX_RESOURCES_IN_FLIGHT> m_windBuffers{};

	};

}
