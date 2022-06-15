#pragma once

#include "IMaterialSlot.h"
#include "DynamicArray.hpp"
#include "Pair.hpp"

namespace OSK::GRAPHICS {

	class GpuUniformBufferDx12;
	class GpuImageDx12;

	class OSKAPI_CALL MaterialSlotDx12 : public IMaterialSlot {

	public:

		MaterialSlotDx12(const std::string& name, const MaterialLayout* layout);

		void SetUniformBuffer(const std::string& binding, const IGpuUniformBuffer* buffer) override;
		void SetGpuImage(const std::string& binding, const GpuImage* image) override;
		void SetStorageBuffer(const std::string& binding, const GpuDataBuffer* buffer) override;
		void SetStorageImage(const std::string& binding, const GpuImage* image) override;
		void SetAccelerationStructure(const std::string& binding, const ITopLevelAccelerationStructure* accelerationStructure) override;
		void FlushUpdate() override;

		const DynamicArray<Pair<TSize, const GpuUniformBufferDx12*>>& GetUniformBuffers() const;
		const DynamicArray<Pair<TSize, const GpuImageDx12*>>& GetGpuImages() const;

	private:

		DynamicArray<Pair<TSize, const GpuUniformBufferDx12*>> buffers;
		DynamicArray<Pair<TSize, const GpuImageDx12*>> images;

	};

}
