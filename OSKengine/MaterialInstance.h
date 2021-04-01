#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "DescriptorSet.h"
#include "Texture.h"

#include "SharedPtr.hpp"

namespace OSK {

	class Material;
	class MaterialPool;

	//Instancia de un material.
	class OSKAPI_CALL MaterialInstance {

		friend class Material;
		friend class MaterialPool;

	public:

		~MaterialInstance();
		void Free();

		void SetTexture(Texture* texture);
		void SetTexture(Texture* texture, uint32_t binding);
		void SetTexture(const std::string& name, Texture* texture);

		void SetBuffer(std::vector<VulkanBuffer>& buffers);
		void SetBuffer(std::vector<VulkanBuffer>& buffers, uint32_t binding);
		void SetBuffer(const std::string& name, std::vector<VulkanBuffer>& buffers);

		void SetDynamicBuffer(std::vector<VulkanBuffer>& buffers);
		void SetDynamicBuffer(std::vector<VulkanBuffer>& buffers, uint32_t binding);
		void SetDynamicBuffer(const std::string& name, std::vector<VulkanBuffer>& buffers);

		void FlushUpdate();

		DescriptorSet* GetDescriptorSet() const;
		bool HasBeenSet() const;

	private:

		Material* OwnerMaterial = nullptr;
		MaterialPool* OwnerPool = nullptr;

		uint32_t DSet = 0;
		bool hasBeenSet = false;
		
	};

}
