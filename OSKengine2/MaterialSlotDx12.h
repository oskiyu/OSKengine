#pragma once

#include "IMaterialSlot.h"
#include "DynamicArray.hpp"
#include "Pair.hpp"

namespace OSK::GRAPHICS {

	class GpuImageDx12;

	class OSKAPI_CALL MaterialSlotDx12 : public IMaterialSlot {

	public:

		MaterialSlotDx12(const std::string& name, const MaterialLayout* layout);

		void SetUniformBuffers(const std::string& binding, const GpuBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]);
		void SetGpuImages(const std::string& binding, const IGpuImageView* image[NUM_RESOURCES_IN_FLIGHT]) override;
		void SetStorageBuffers(const std::string& binding, const GpuBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]) override;
		void SetStorageImages(const std::string& binding, const IGpuImageView* image[NUM_RESOURCES_IN_FLIGHT]) override;
		void SetAccelerationStructures(const std::string& binding, const ITopLevelAccelerationStructure* accelerationStructure[NUM_RESOURCES_IN_FLIGHT]) override;
		void FlushUpdate() override;

		const DynamicArray<Pair<TSize, const GpuBuffer*>>& GetUniformBuffers() const;
		const DynamicArray<Pair<TSize, const GpuImageDx12*>>& GetGpuImages() const;

		const DynamicArray<Pair<TSize, const GpuBuffer*>>& GetStorageBuffers() const;
		const DynamicArray<Pair<TSize, const GpuImageDx12*>>& GetStorageImages() const;

		void SetDebugName(const std::string& name) override;

	private:

		DynamicArray<Pair<TSize, const GpuBuffer*>> buffers;
		DynamicArray<Pair<TSize, const GpuImageDx12*>> images;

		DynamicArray<Pair<TSize, const GpuBuffer*>> storageBuffers;
		DynamicArray<Pair<TSize, const GpuImageDx12*>> storageImages;

	};

}
