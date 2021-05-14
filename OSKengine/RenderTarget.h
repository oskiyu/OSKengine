#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Framebuffer.h"
#include "Renderpass.h"
#include "GPUImage.h"
#include "GraphicsPipeline.h"

#include "Sprite.h"
#include "ContentManager.h"
#include "Renderpass.h"

#include <vector>

namespace OSK {

	class RenderAPI;

	/// <summary>
	/// Render target simboliza la imagen sobre la que se va a renderizar una escena 3D o un render stage, por ejemplo.
	/// Contiene las imágenes, el renderpass y los framebuffers necesarios para el renderizado.
	/// También permite renderizar la imagen final como un sprite.
	/// </summary>
	class OSKAPI_CALL RenderTarget {

		friend class RenderAPI;
		friend class RenderizableScene;
		friend class RenderSystem3D;

	public:

		/// <summary>
		/// Elimina el rendertarget.
		/// </summary>
		~RenderTarget();

		/// <summary>
		/// Establece el formato que va a usar la imagen final.
		/// </summary>
		/// <param name="format">Formato de la imagen.</param>
		void SetFormat(VkFormat format);

		/// <summary>
		/// Crea el renderpass.
		/// </summary>
		/// <param name="colorAttachments">Información del color.</param>
		/// <param name="depthAttachment">Información del depth / stencil.</param>
		/// <param name="msaa">Samples del MSAA.</param>
		void CreateRenderpass(std::vector<VULKAN::RenderpassAttachment> colorAttachments, VULKAN::RenderpassAttachment* depthAttachment, VkSampleCountFlagBits msaa = VK_SAMPLE_COUNT_1_BIT);

		/// <summary>
		/// Crea los framebuffers.
		/// </summary>
		/// <param name="numFb">Número de framebuffers a crear.</param>
		/// <param name="images">Array de imágenes que va a usar.</param>
		/// <param name="numViews">Número de imágenes que va a usar.</param>
		void CreateFramebuffers(uint32_t numFb, VkImageView* images, uint32_t numViews);

		/// <summary>
		/// Establece el tamaño de la imagen renderizada.
		/// </summary>
		/// <param name="sizeX">Tamaño en X.</param>
		/// <param name="sizeY">Tamaño en Y.</param>
		/// <param name="createColorImage">True si (re)crea las imágenes.</param>
		void SetSize(uint32_t sizeX, uint32_t sizeY, bool createColorImage = true);

		/// <summary>
		/// Crea el sprite para renderizar la imagen final como sprite.
		/// No es estrictamente necesario.
		/// </summary>
		/// <param name="content">Content manager que va a almacenar el sprite.</param>
		void CreateSprite(ContentManager* content);

		/// <summary>
		/// Prepara el render target para renderizar sobre él.
		/// </summary>
		/// <param name="cmdBuffer">Command buffer.</param>
		/// <param name="layout">Layout (color por defecto).</param>
		void TransitionToRenderTarget(VkCommandBuffer* cmdBuffer, VkImageLayout layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		/// <summary>
		/// Prepara el render target para ser usado como sprite / textura.
		/// </summary>
		/// <param name="cmdBuffer">Command buffer.</param>
		void TransitionToTexture(VkCommandBuffer* cmdBuffer);

		/// <summary>
		/// Resetea el render target.
		/// </summary>
		/// <param name="complete">También elimina el renderpass.</param>
		void Clear(bool complete = true);

		/// <summary>
		/// Sprite que contiene la textura renderizada.
		/// </summary>
		Sprite renderedSprite{};

		/// <summary>
		/// Tamaño de la imagen renderizada.
		/// </summary>
		Vector2ui GetSize();

	private:

		/// <summary>
		/// Crea un render target vacío.
		/// </summary>
		/// <param name="renderer">Renderizador.</param>
		RenderTarget(RenderAPI* renderer);

		/// <summary>
		/// Content manager que contiene el sprite.
		/// </summary>
		ContentManager* content = nullptr;

		/// <summary>
		/// Renderizador.
		/// </summary>
		RenderAPI* renderer = nullptr;

		/// <summary>
		/// True si el sprite puede usarse.
		/// </summary>
		bool spriteHasBeenCreated = false;

		/// <summary>
		/// Formato de la imagen.
		/// </summary>
		VkFormat format;

		/// <summary>
		/// Framebuffers.
		/// </summary>
		std::vector<VULKAN::Framebuffer*> targetFramebuffers;

		/// <summary>
		/// Renderpass.
		/// </summary>
		VULKAN::Renderpass* vulkanRenderpass = nullptr;

		/// <summary>
		/// Tamaño de la imagen renderizada.
		/// </summary>
		Vector2ui size;

	};

}
