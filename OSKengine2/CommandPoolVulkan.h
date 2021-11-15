#pragma once

#include "ICommandPool.h"

struct VkCommandPool_T;
typedef VkCommandPool_T* VkCommandPool;

namespace OSK {

	/// <summary>
	/// Una pool de comandos se encarga de crear una serie de listas de comandos.
	/// 
	/// Esta es la implementación del pool para el renderizador de Vulkan.
	/// </summary>
	class OSKAPI_CALL CommandPoolVulkan : public ICommandPool {

	public:

		/// <summary>
		/// Crea una nueva lista de comandos.
		/// </summary>
		OwnedPtr<ICommandList> CreateCommandList(const IGpu& device) override;

		/// <summary>
		/// Establece el número de imágenes del swapchain.
		/// 
		/// Cada imagen tiene su propia lista de comandos.
		/// La lista de comandos contendrá varias listas nativas.
		/// </summary>
		void SetSwapchainCount(unsigned int count);

		/// <summary>
		/// Obtiene el número de imágenes del swapchain.
		/// 
		/// Cada imagen tiene su propia lista de comandos.
		/// La lista de comandos contendrá varias listas nativas.
		/// </summary>
		unsigned int GetSwapchainCount() const;

		void SetCommandPool(VkCommandPool commandPool);
		VkCommandPool GetCommandPool() const;

	private:

		unsigned int numberOfImages = 0;
		VkCommandPool commandPool;

	};

}
