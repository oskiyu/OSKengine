#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>
#include <vector>

#include "DescriptorLayout.h"
#include "DescriptorPool.h"
#include "GPUDataBuffer.h"
#include "GPUImage.h"

namespace OSK {

	class GraphicsPipeline;

	/// <summary>
	/// Un descriptor set describe, de manera concisa, c�mo se pasa la informaci�n de la CPU a la GPU de un objeto en concreto.
	/// Tiene informaci�n extra, como detalles sobre los buffers y las im�genes.
	/// Encapsula un descriptor set nativo de Vulkan.
	/// </summary>
	class OSKAPI_CALL DescriptorSet {

		friend class RenderAPI;

	public:

		/// <summary>
		/// Destruye el descriptor set.
		/// </summary>
		~DescriptorSet();

		/// <summary>
		/// Establece el descriptor layout que define la comunicaci�n CPU->GPU que va a usar este descriptor set.
		/// </summary>
		/// <param name="layout">Layout del descriptor set.</param>
		/// <param name="pool">Pool owner del descriptor set.</param>
		/// <param name="allocate">True si se va a crear un nuevo descriptor set.</param>
		void Create(DescriptorLayout* layout, DescriptorPool* pool, bool allocate = true);

		/// <summary>
		/// Registra un buffer que va a usarse como un UBO.
		/// </summary>
		/// <param name="buffers">Buffers que almacenan la informaci�n que se usar� en los shaders.</param>
		/// <param name="binding">Binding del UBO en el shader.</param>
		/// <param name="size">Tama�o del buffer.</param>
		void AddUniformBuffers(std::vector<GPUDataBuffer> buffers, uint32_t binding, size_t size);

		/// <summary>
		/// Registra un buffer que va a usarse como un dynamic UBO.
		/// </summary>
		/// <param name="buffers">Buffers que almacenan la informaci�n que se usar� en los shaders.</param>
		/// <param name="binding">Binding del dynamic UBO en el shader.</param>
		/// <param name="size">Tama�o del buffer.</param>
		void AddDynamicUniformBuffers(std::vector<GPUDataBuffer> buffers, uint32_t binding, size_t size);

		/// <summary>
		/// Registra una imagen.
		/// </summary>
		/// <param name="image">Imagen.</param>
		/// <param name="sampler">Sampler (no tiene por que ser el de la imagen).</param>
		/// <param name="binding">Binding en el shader.</param>
		void AddImage(VULKAN::GPUImage* image, VkSampler sampler, uint32_t binding);

		/// <summary>
		/// Crea o actualiza el descriptor set.
		/// IMPORTANTE: debe llamarse antes a SetDescriptorLayout.
		/// </summary>
		void Update();

		/// <summary>
		/// Establece este descriptor set como el que se va a usar a partir de ahora en un CommandBuffer.
		/// </summary>
		/// <param name="commandBuffer">CommandBuffer.</param>
		/// <param name="pipeline">Pipeline con el que se est� renderizando.</param>
		/// <param name="iteration">Qu� VulkanDescriptorSets se va a usar.</param>
		void Bind(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline, uint32_t iteration) const;

		/// <summary>
		/// Establece este descriptor set como el que se va a usar a partir de ahora en un CommandBuffer.
		/// </summary>
		/// <param name="commandBuffer">CommandBuffer.</param>
		/// <param name="pipeline">Pipeline con el que se est� renderizando.</param>
		/// <param name="iteration">Qu� VulkanDescriptorSets se va a usar.</param>
		/// <param name="dynamicOffset">Offset de la entidad del dynamic UBO.</param>
		/// <param name="alignment">Alignment del dynamic ubo.</param>
		void Bind(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline, uint32_t iteration, uint32_t dynamicOffset, uint32_t alignment) const;

		/// <summary>
		/// Descriptor sets nativos de Vulkan.
		/// </summary>
		std::vector<VkDescriptorSet> VulkanDescriptorSets;

		/// <summary>
		/// Restea la informaci�n de los bindings.
		/// </summary>
		void Reset();

		/// <summary>
		/// Obtiene el n�mero de bindings que han sido establecidos.
		/// </summary>
		/// <returns>N�mero de bindings.</returns>
		uint32_t GetBindingsCount() const;

	private:

		/// <summary>
		/// Crea un descriptor se vac�o.
		/// </summary>
		/// <param name="logicalDevice">Logical device del renderizador.</param>
		/// <param name="swapchainCount">N�mero de im�genes del swapchain del renderizador.</param>
		DescriptorSet(VkDevice logicalDevice, uint32_t swapchainCount);

		/// <summary>
		/// Elimina la informaci�n nativa de los bindings.
		/// Se llama tras establecer el DescriptorSet.
		/// </summary>
		void clear();

		/// <summary>
		/// Layout del descriptor set.
		/// </summary>
		DescriptorLayout* layout = nullptr;

		/// <summary>
		/// Descriptor pool, owner de este descriptor set.
		/// </summary>
		DescriptorPool* pool = nullptr;

		/// <summary>
		/// Informaci�n nativa de los bindings.
		/// </summary>
		std::vector<std::vector<VkWriteDescriptorSet>> descriptorWrites{};

		/// <summary>
		/// Logical device del renderizador.
		/// </summary>
		VkDevice logicalDevice;

		/// <summary>
		/// N�mero de im�genes del swapchain.
		/// </summary>
		uint32_t swapchainCount = 0;

		/// <summary>
		/// Informaci�n nativa de los bindings: buffers.
		/// </summary>
		std::vector<VkDescriptorBufferInfo*> bufferInfos{};

		/// <summary>
		/// Informaci�n nativa de los bindings: im�genes.
		/// </summary>
		std::vector<VkDescriptorImageInfo*> imageInfos{};

		/// <summary>
		/// True si se han liampiado las listas de informaci�n nativa del descriptor set.
		/// </summary>
		bool cleared = false;

		/// <summary>
		/// N�mero de bindings que ya han sido establecidos.
		/// </summary>
		uint32_t bindingsCount = 0;

	};

}