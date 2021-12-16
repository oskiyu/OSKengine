#pragma once

#include "ICommandList.h"
#include "DynamicArray.hpp"

struct VkCommandBuffer_T;
typedef VkCommandBuffer_T* VkCommandBuffer;

namespace OSK {

	/// <summary>
	/// Una lista de comandos contiene una serie de comandos que ser�n
	/// enviados a la GPU para su ejecuci�n.
	/// La lista es creada por una pool de comandos, y se introduce
	/// en una cola de comandos para su ejecuci�n.
	/// 
	/// Esta es la implementaci�n de la lista de comandos para el 
	/// renderizador de Vulkan.
	/// 
	/// El n�mero de listas de comandos nativas depender� del n�mero de
	/// im�genes en el swapchain.
	/// Esta clase representa una abstracci�n de una lista de comandos:
	/// cada comando grabado en la clase se grabar� en cada una de las
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

		void TransitionImageLayout(GpuImage* image, GpuImageLayout previous, GpuImageLayout next) override;

	private:

		/// <summary>
		/// Varias listas nativas, una por cada imagen en el swapchain.
		/// </summary>
		DynamicArray<VkCommandBuffer> commandBuffers;

	};

}
