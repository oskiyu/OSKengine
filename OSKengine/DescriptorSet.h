#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "DescriptorLayout.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "GraphicsPipeline.h"

namespace OSK {

	//Un descriptor set describe, de manera concisa, c�mo se pasa la informaci�n de la CPU a la GPU de un objeto en concreto.
	//Tiene informaci�n extra, como detalles sobre los buffers y las im�genes.
	class DescriptorSet {

		friend class VulkanRenderer;

	public:

		//Destruye el descriptor set.
		~DescriptorSet();

		//Establece el descriptor layout que define la comunicaci�n CPU->GPU que va a usar este descriptor set.
		void SetDescriptorLayout(DescriptorLayout* layout);

		//Registra un UBO.
		//	<buffers>: buffers que almacenan la informaci�n que se usar� en los shaders.
		//	<binding>: binding del UBO en el shader.
		//	<size>: tama�o del UBO.
		void AddUniformBuffers(std::vector<VulkanBuffer> buffers, const uint32_t& binding, const size_t& size);

		//Registra una imagen.
		//	<image>: imagen que se usar� en los shaders.
		//	<sampler>: sampler de la imagen.
		//	<binding>: binding de la imagen en el shader.
		void AddImage(VULKAN::VulkanImage* image, VkSampler sampler, const uint32_t& binding);

		//Crea el descriptor set.
		//IMPORTANTE: debe llamarse antes a SetDescriptorLayout.
		void Create();

		//Establece este descriptor set como el que se va a usar a partir de ahora en un CommandBuffer.
		//	<commandBuffer>: CommandBuffer.
		//	<pipeline>: pipeline con el que se est� renderizando.
		//	<iteration>: qu� VulkanDescriptorSets se va a usar.
		void Bind(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline, const uint32_t& iteration) const;

		std::vector<VkDescriptorSet> VulkanDescriptorSets;

	private:

		DescriptorSet(VkDevice logicalDevice, const uint32_t& swapchainCount);

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