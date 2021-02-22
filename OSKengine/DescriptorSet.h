#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>
#include <vector>

#include "DescriptorLayout.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"

namespace OSK {

	class GraphicsPipeline;

	//Un descriptor set describe, de manera concisa, cómo se pasa la información de la CPU a la GPU de un objeto en concreto.
	//Tiene información extra, como detalles sobre los buffers y las imágenes.
	class OSKAPI_CALL DescriptorSet {

		friend class RenderAPI;

	public:

		//Destruye el descriptor set.
		~DescriptorSet();

		//Establece el descriptor layout que define la comunicación CPU->GPU que va a usar este descriptor set.
		void SetDescriptorLayout(DescriptorLayout* layout);

		//Registra un UBO.
		//	<buffers>: buffers que almacenan la información que se usará en los shaders.
		//	<binding>: binding del UBO en el shader.
		//	<size>: tamaño del UBO.
		void AddUniformBuffers(std::vector<VulkanBuffer> buffers, uint32_t binding, size_t size);

		//Registra una imagen.
		//	<image>: imagen que se usará en los shaders.
		//	<sampler>: sampler de la imagen.
		//	<binding>: binding de la imagen en el shader.
		void AddImage(VULKAN::VulkanImage* image, VkSampler sampler, uint32_t binding);

		//Crea el descriptor set.
		//IMPORTANTE: debe llamarse antes a SetDescriptorLayout.
		void Create();

		//Establece este descriptor set como el que se va a usar a partir de ahora en un CommandBuffer.
		//	<commandBuffer>: CommandBuffer.
		//	<pipeline>: pipeline con el que se está renderizando.
		//	<iteration>: qué VulkanDescriptorSets se va a usar.
		void Bind(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline, uint32_t iteration) const;

		std::vector<VkDescriptorSet> VulkanDescriptorSets;

	private:

		DescriptorSet(VkDevice logicalDevice, uint32_t swapchainCount);

		void clear();

		DescriptorLayout* layout;
		std::vector<std::vector<VkWriteDescriptorSet>> descriptorWrites{};

		VkDevice logicalDevice;
		uint32_t swapchainCount;

		std::vector<VkDescriptorBufferInfo*> bufferInfos{};
		std::vector<VkDescriptorImageInfo*> imageInfos{};

		bool cleared = false;

	};

}