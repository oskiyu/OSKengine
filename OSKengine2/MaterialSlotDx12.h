#pragma once

#include "IMaterialSlot.h"
#include "DynamicArray.hpp"
#include "Pair.hpp"

namespace OSK::GRAPHICS {

	class GpuImageDx12;

	class OSKAPI_CALL MaterialSlotDx12 : public IMaterialSlot {

	public:

		MaterialSlotDx12(const std::string& name, const MaterialLayout* layout);

		void SetUniformBuffers(const std::string& binding, std::span<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT>, UIndex32 arrayIndex) override;
		void SetGpuImages(const std::string& binding, std::span<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT>, UIndex32 arrayIndex) override;
		void SetStorageBuffers(const std::string& binding, std::span<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT>, UIndex32 arrayIndex) override;
		void SetStorageImages(const std::string& binding, std::span<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT>, UIndex32 arrayIndex) override;
		void SetAccelerationStructures(const std::string& binding, std::span<const ITopLevelAccelerationStructure*, NUM_RESOURCES_IN_FLIGHT>, UIndex32 arrayIndex) override;
		void FlushUpdate() override;

		const DynamicArray<Pair<USize32, const GpuBuffer*>>& GetUniformBuffers() const;
		const DynamicArray<Pair<USize32, const GpuImageDx12*>>& GetGpuImages() const;

		const DynamicArray<Pair<USize32, const GpuBuffer*>>& GetStorageBuffers() const;
		const DynamicArray<Pair<USize32, const GpuImageDx12*>>& GetStorageImages() const;

		void SetDebugName(const std::string& name) override;

	private:

		DynamicArray<Pair<USize32, const GpuBuffer*>> buffers;
		DynamicArray<Pair<USize32, const GpuImageDx12*>> images;

		DynamicArray<Pair<USize32, const GpuBuffer*>> storageBuffers;
		DynamicArray<Pair<USize32, const GpuImageDx12*>> storageImages;

	};

}
