#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>

#include <vector>

#include "RenderpassAttachment.h"
#include "SubpassDependency.h"

namespace OSK::VULKAN {

	//Representa un subpass.

	/// <summary>
	/// Representa un subpass:
	/// una aprte del renderpass.
	/// </summary>
	class OSKAPI_CALL RenderpassSubpass {

	public:

		/// <summary>
		/// Destruye el subpass.
		/// </summary>
		~RenderpassSubpass();

		/// <summary>
		/// Establece en que parte se va a aplicar el subpass.
		/// </summary>
		/// <param name="point">Punto en el que se va a usar.</param>
		void SetPipelineBindPoint(VkPipelineBindPoint point = VK_PIPELINE_BIND_POINT_GRAPHICS);

		/// <summary>
		/// Establece los attachments de las imágenes que representarán el color.
		/// </summary>
		/// <param name="attachments">Attachments de color.</param>
		void SetColorAttachments(const std::vector<RenderpassAttachment>& attachments);

		/// <summary>
		/// Establece el attachment de las imagen que representarán el depth/stencil.
		/// </summary>
		/// <param name="attachment">Attachments de depth / stencil.</param>
		void SetDepthStencilAttachment(const RenderpassAttachment& attachment);

		/// <summary>
		/// Añade una dependencia al renderpass.
		/// </summary>
		/// <param name="dependency">Dependencia.</param>
		void AddDependency(SubpassDependency dependency);

		/// <summary>
		/// Descripción de Vulkan del subpass.
		/// </summary>
		VkSubpassDescription description;

		/// <summary>
		/// Referencias de los attachments.
		/// </summary>
		std::vector<VkAttachmentReference> references{};

		/// <summary>
		/// Dependencias.
		/// </summary>
		std::vector<SubpassDependency> dependencies{};

	};

}
