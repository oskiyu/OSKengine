#pragma once

#include "IRenderSystem.h"

#include "UniquePtr.hpp"
#include "IRenderpass.h"
#include "IGpuImage.h"

namespace OSK::ECS {

	class OSKAPI_CALL PbrDeferredRenderSystem : public IRenderSystem {

	public:

		OSK_SYSTEM("OSK::PbrDeferredRenderSystem");

		PbrDeferredRenderSystem();

		void Render(GRAPHICS::ICommandList* commandList) override;

		void CreateTargetImage(const Vector2ui& size) override;
		void Resize(const Vector2ui& windowSize) override;

	private:
		
		void ExecuteNormalRenderpass(GRAPHICS::ICommandList* commandList);
		void ExecuteColorRenderpass(GRAPHICS::ICommandList* commandList);

		UniquePtr<GRAPHICS::IRenderpass> normalRenderpass;
		UniquePtr<GRAPHICS::IRenderpass> colorRenderpass;

		UniquePtr<GRAPHICS::GpuImage> normalImage;
		UniquePtr<GRAPHICS::GpuImage> colorImage;

	};

}
