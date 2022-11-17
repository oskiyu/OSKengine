#pragma once

#include "IPostProcessPass.h"

namespace OSK::GRAPHICS {

	class ICommandList;

	class OSKAPI_CALL SmaaPass : public IPostProcessPass {

	public:

		void Create(const Vector2ui& size) override;
		void Resize(const Vector2ui& size) override;

		void Execute(ICommandList* computeCmdList) override;

	private:

		void SetupMaterials();

		UniquePtr<GpuImage> edgeImages[3]{};

	};

}
