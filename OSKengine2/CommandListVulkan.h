#pragma once

#include "ICommandList.h"
#include <vector>

struct VkCommandBuffer_T;
typedef VkCommandBuffer_T* VkCommandBuffer;

namespace OSK {

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

		const std::vector<VkCommandBuffer>& GetCommandBuffers() const;
		std::vector<VkCommandBuffer>* GetCommandBuffers();

	private:

		std::vector<VkCommandBuffer> commandBuffers;

	};

}
