#pragma once

#include "IPostProcessPass.h"

namespace OSK::GRAPHICS {

	class ICommandList;

	class OSKAPI_CALL FxaaPass : public IPostProcessPass {

	public:

		void Create(const Vector2ui& size) override;

		void Execute(ICommandList* computeCmdList) override;

	};

}
