#pragma once

#include "IMaterialSlot.h"
#include "HashMap.hpp"

namespace OSK {

	class GpuUniformBufferDx12;

	class OSKAPI_CALL MaterialSlotDx12 : public IMaterialSlot {

	public:

		MaterialSlotDx12(const std::string& name, const MaterialLayout* layout);

		void SetUniformBuffer(const std::string& binding, const IGpuUniformBuffer* buffer) override;
		void FlushUpdate() override;

		const HashMap<TSize, GpuUniformBufferDx12*>& GetUniformBuffers() const;

	private:

		HashMap<TSize, GpuUniformBufferDx12*> buffers;

	};

}
