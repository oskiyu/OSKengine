#pragma once

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "IMaterialSlot.h"
#include "DynamicArray.hpp"
#include "Pair.hpp"

namespace OSK::GRAPHICS {

	class GpuImageDx12;

	class OSKAPI_CALL MaterialSlotDx12 : public IMaterialSlot {

	public:

		MaterialSlotDx12(const std::string& name, const MaterialLayout* layout);

		void SetUniformBuffer(std::string_view binding, const GpuBuffer&, const GpuBufferRange& range, UIndex32 arrayIndex) override;
		void SetGpuImage(std::string_view binding, const IGpuImageView& view, const IGpuImageSampler& sampler, UIndex32 arrayIndex) override;
		void SetStorageBuffer(std::string_view binding, const GpuBuffer&, const GpuBufferRange& range, UIndex32 arrayIndex) override;
		void SetStorageImage(std::string_view binding, const IGpuImageView&, UIndex32 arrayIndex) override;
		void SetAccelerationStructure(std::string_view binding, const ITopLevelAccelerationStructure&, UIndex32 arrayIndex) override;
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

#endif
