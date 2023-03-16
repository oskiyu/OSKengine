#pragma once

#include "IMaterialSlot.h"
#include "DynamicArray.hpp"
#include "Pair.hpp"

namespace OSK::GRAPHICS {

	class GpuUniformBufferDx12;
	class GpuImageDx12;
	class IGpuStorageBuffer;

	class OSKAPI_CALL MaterialSlotDx12 : public IMaterialSlot {

	public:

		MaterialSlotDx12(const std::string& name, const MaterialLayout* layout);

		void SetUniformBuffer(const std::string& binding, const IGpuUniformBuffer* buffer) override;
		void SetUniformBuffers(const std::string& binding, const IGpuUniformBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]);
		void SetGpuImage(const std::string& binding, const IGpuImageView* image) override;
		void SetGpuImages(const std::string& binding, const IGpuImageView* image[NUM_RESOURCES_IN_FLIGHT]) override;
		void SetStorageBuffer(const std::string& binding, const GpuDataBuffer* buffer) override;
		void SetStorageBuffers(const std::string& binding, const GpuDataBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]) override;
		void SetStorageImage(const std::string& binding, const IGpuImageView* image) override;
		void SetStorageImages(const std::string& binding, const IGpuImageView* image[NUM_RESOURCES_IN_FLIGHT]) override;
		void SetAccelerationStructure(const std::string& binding, const ITopLevelAccelerationStructure* accelerationStructure) override;
		void SetAccelerationStructures(const std::string& binding, const ITopLevelAccelerationStructure* accelerationStructure[NUM_RESOURCES_IN_FLIGHT]) override;
		void FlushUpdate() override;

		const DynamicArray<Pair<TSize, const GpuUniformBufferDx12*>>& GetUniformBuffers() const;
		const DynamicArray<Pair<TSize, const GpuImageDx12*>>& GetGpuImages() const;

		const DynamicArray<Pair<TSize, const IGpuStorageBuffer*>>& GetStorageBuffers() const;
		const DynamicArray<Pair<TSize, const GpuImageDx12*>>& GetStorageImages() const;

		void SetDebugName(const std::string& name) override;

	private:

		DynamicArray<Pair<TSize, const GpuUniformBufferDx12*>> buffers;
		DynamicArray<Pair<TSize, const GpuImageDx12*>> images;

		DynamicArray<Pair<TSize, const IGpuStorageBuffer*>> storageBuffers;
		DynamicArray<Pair<TSize, const GpuImageDx12*>> storageImages;

	};

}
