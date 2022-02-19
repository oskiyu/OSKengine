#pragma once

#include "ICommandList.h"
#include "DynamicArray.hpp"

struct VkCommandBuffer_T;
typedef VkCommandBuffer_T* VkCommandBuffer;

namespace OSK {

	class GpuDataBuffer;
	class GraphicsPipelineVulkan;
	class Material;

	/// <summary>
	/// Una lista de comandos contiene una serie de comandos que serán
	/// enviados a la GPU para su ejecución.
	/// La lista es creada por una pool de comandos, y se introduce
	/// en una cola de comandos para su ejecución.
	/// 
	/// Esta es la implementación de la lista de comandos para el 
	/// renderizador de Vulkan.
	/// 
	/// El número de listas de comandos nativas dependerá del número de
	/// imágenes en el swapchain.
	/// Esta clase representa una abstracción de una lista de comandos:
	/// cada comando grabado en la clase se grabará en cada una de las
	/// listas nativas.
	/// </summary>
	class OSKAPI_CALL CommandListVulkan : public ICommandList {

	public:

		const DynamicArray<VkCommandBuffer>& GetCommandBuffers() const;
		DynamicArray<VkCommandBuffer>* GetCommandBuffers();

		void Reset() override;
		void Start() override;
		void Close() override;

		void BeginRenderpass(IRenderpass* renderpass) override;
		void BeginAndClearRenderpass(IRenderpass* renderpass, const Color& color) override;
		void EndRenderpass(IRenderpass* renderpass) override;

		void BindMaterial(const Material* material) override;
		void BindVertexBuffer(IGpuVertexBuffer* buffer) override;
		void BindIndexBuffer(IGpuIndexBuffer* buffer) override;
		void BindMaterialSlot(const IMaterialSlot* slot) override;

		void TransitionImageLayout(GpuImage* image, GpuImageLayout previous, GpuImageLayout next) override;

		/// <summary>
		/// Copia los contenidos del primer buffer al segundo.
		/// </summary>
		/// <param name="source">Buffer donde están el contenido que se quiere copiar.</param>
		/// <param name="dest">Buffer en el que se va a poner el contenido copiado.</param>
		/// <param name="size">Tamaño, en bytes, del área de memoria que se va a copiar.</param>
		/// <param name="sourceOffset">Offset del área de memoria en el origen.</param>
		/// <param name="destOffset">Offset del área de memoria en el destino.</param>
		void CopyBuffer(const GpuDataBuffer* source, GpuDataBuffer* dest, TSize size, TSize sourceOffset, TSize destOffset);

		void SetViewport(const Viewport& viewport) override;
		void SetScissor(const Vector4ui& scissor) override;

	private:

		/// <summary>
		/// Varias listas nativas, una por cada imagen en el swapchain.
		/// </summary>
		DynamicArray<VkCommandBuffer> commandBuffers;

	};

}
