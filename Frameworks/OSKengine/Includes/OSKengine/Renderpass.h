#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>

#include "RenderpassAttachment.h"
#include "RenderpassSubpass.h"

#include <vector>

namespace OSK::VULKAN {
	
	/// <summary>
	/// Representa un renderpass.
	/// Un renderpass representa el target sobre el cual se renderiza una imagen.
	/// </summary>
	class OSKAPI_CALL Renderpass {

		friend class GraphicsPipeline;
		friend class RenderAPI;

	public:

		/// <summary>
		/// Destruye el renderpass.
		/// </summary>
		~Renderpass();

		/// <summary>
		/// Establece el MSAA a usar.
		/// </summary>
		/// <param name="samples">Samples de MSAA.</param>
		void SetMSAA(VkSampleCountFlagBits samples);

		/// <summary>
		/// Añade un attachment al renderpass.
		/// </summary>
		/// <param name="attachment">Attachment.</param>
		void AddAttachment(RenderpassAttachment attachment);

		/// <summary>
		/// Añade un subpass al renderpass.
		/// </summary>
		/// <param name="subpass">Subpass.</param>
		void AddSubpass(RenderpassSubpass subpass);

		/// <summary>
		/// Crea el renderpass.
		/// </summary>
		void Create();

		/// <summary>
		/// Renderpass nativo.
		/// </summary>
		VkRenderPass VulkanRenderpass = VK_NULL_HANDLE;

	private:

		/// <summary>
		/// Crea un renderpass vacío.
		/// </summary>
		/// <param name="logicalDevice">Logical device del renderizador.</param>
		Renderpass(VkDevice logicalDevice);

		/// <summary>
		/// Attachments.
		/// </summary>
		std::vector<RenderpassAttachment> attachments{};

		/// <summary>
		/// Subpasses.
		/// </summary>
		std::vector<RenderpassSubpass> subpasses;

		/// <summary>
		/// Samples del MSAA.
		/// </summary>
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

		/// <summary>
		/// Logical device del renderizador.
		/// </summary>
		VkDevice logicalDevice = VK_NULL_HANDLE;
		
		/// <summary>
		/// Formato del swapchain objetivo.
		/// </summary>
		VkFormat swapchainFormat;

	};

}