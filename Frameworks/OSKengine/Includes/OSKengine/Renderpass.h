#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace OSK::VULKAN {

	//Representa los attachments del renderpass.
	class OSKAPI_CALL RenderpassAttachment {

	public:

		//Establece el attachment.
		//	<format>: formato.
		//	<loadOp>: qué se hará al cargar la imagen.
		//	<stroeOp>: qué se hará al guardar la imagen.
		//	<layout>: layout de la imagen.
		void AddAttachment(VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp stroeOp, VkImageLayout layout);

		//Crea la referencia del attachment,
		//	<attachment>: número del attachment.
		//	<use>: layout que tendrá.
		void CreateReference(const uint32_t& attachment, VkImageLayout use);

		VkAttachmentDescription attahcmentDesc;
		VkAttachmentReference reference;

	};


	class OSKAPI_CALL SubpassDependency {

	public:

		VkSubpassDependency VulkanDependency;

	};


	//Representa un subpass.
	class OSKAPI_CALL RenderpassSubpass {

	public:

		//Destruye el subpass.
		~RenderpassSubpass();

		//Establece en que parte se va a aplicar el subpass.
		//	<point>: punto en el que se va a usar.
		void SetPipelineBindPoint(VkPipelineBindPoint point = VK_PIPELINE_BIND_POINT_GRAPHICS);

		//Establece los attachments de las imágenes que representarán el color.
		//	<attachments>: attachments.
		void SetColorAttachments(const std::vector<RenderpassAttachment>& attachments);

		//Establece el attachment de las imagen que representarán el depth/stencil..
		//	<attachment>: attachment.
		void SetDepthStencilAttachment(const RenderpassAttachment& attachment);

		//Añade una dependencia al renderpass.
		void AddDependency(SubpassDependency dependency);

		//Crea el subpass.
		//	<srcSubpass>: subpass anterior a este.
		//	<dstSubpass>: subpass posterior a este.
		//	<sourceStageMask>: sourceStageMask.
		//	<sourceAccess>: sourceAccess.
		//	<destStageMask>: sourceAccess.
		//	<destAccess>: sourceAccess.
		void Set(/*const uint32_t& srcSubpass, const uint32_t& dstSubpass, VkPipelineStageFlags sourceStageMask, VkAccessFlags sourceAccess, VkPipelineStageFlags destStageMask, VkAccessFlags destAccess*/);

		VkSubpassDescription description;

		std::vector<VkAttachmentReference> references{};
		std::vector<SubpassDependency> dependencies{};

	};


	//Representa un renderpass.
	class OSKAPI_CALL Renderpass {

		friend class GraphicsPipeline;
		friend class RenderAPI;

	public:

		//Destruye el renderpass.
		~Renderpass();

		//Establece el MSAA a usar.
		//	<samples>: samples de MSAA.
		void SetMSAA(VkSampleCountFlagBits samples);

		//Añade un attachment al renderpass.
		//	<attachment>: attachment.
		void AddAttachment(RenderpassAttachment attachment);

		//Añade un subpass al renderpass.
		//	<subpass>: subpass.
		void AddSubpass(RenderpassSubpass subpass);

		//Crea el renderpass.
		void Create();

		VkRenderPass VulkanRenderpass;

	private:

		Renderpass(VkDevice logicalDevice);

		//Config:
		std::vector<RenderpassAttachment> attachments{};

		std::vector<RenderpassSubpass> subpasses;

		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

		VkDevice logicalDevice;
		
		VkFormat swapchainFormat;

	};

}