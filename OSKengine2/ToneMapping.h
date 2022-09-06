#pragma once

#include "IPostProcessPass.h"

namespace OSK::GRAPHICS {

	class ICommandList;
	class IGpuStorageBuffer;

	class OSKAPI_CALL ToneMappingPass : public IPostProcessPass {

	public:

		void Create(const Vector2ui& size) override;

		void Execute(ICommandList* computeCmdList) override;

		void SetExposureBuffers(const IGpuStorageBuffer*[3]);

		void SetExposure(float exposure);
		float GetExposure() const;

		void SetGamma(float gamma);
		float GetGamma() const;

	private:

		float exposure = 4.0f;
		float gamma = 2.2f;

	};

}
