#pragma once

#include "ECS.h"

#include "RenderizableScene.h"
#include "RenderStage.h"

namespace OSK {

	class RenderSystem3D : public ECS::System {

	public:
				
		void Init();
		void OnDraw(VkCommandBuffer cmdBuffer, uint32_t i) override;
		void OnTick(deltaTime_t deltaTime) override;
		void OnRemove() override;
		Signature GetSystemSignature() override;

		RenderAPI* Renderer = nullptr;

		RenderizableScene* RScene = nullptr;
		RenderStage Stage;

	};

}
