#pragma once

#include "IPostProcessPass.h"

namespace OSK::GRAPHICS {

	class ICommandList;
	class IGpuStorageBuffer;

	class OSKAPI_CALL ToneMappingPass : public IPostProcessPass {

	public:

		void Create(const Vector2ui& size) override;

		void Execute(ICommandList* computeCmdList) override;

		void SetExposureBuffers(std::span<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT>);

		void SetExposure(float exposure);
		float GetExposure() const;

		void SetGamma(float gamma);
		float GetGamma() const;

	private:

		float exposure = 6.0f;
		float gamma = 2.2f;

	};

}
