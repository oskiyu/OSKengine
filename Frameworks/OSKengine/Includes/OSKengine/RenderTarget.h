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
	/// Representa qu� parte de la imagen renderizada ser� considerada la textura del render target.
	/// </summary>
	enum class RenderTargetImageTarget {
		COLOR,
		DEPTH
	};

	/// <summary>
	/// Render target simboliza la imagen sobre la que se va a renderizar una escena 3D o un render stage, por ejemplo.
	/// Contiene las im�genes, el renderpass y los framebuffers necesarios para el renderizado.
	/// Tambi�n permite renderizar la imagen final como un sprite.
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
		/// Crea el renderpass.
		/// </summary>
		/// <param name="colorAttachments">Informaci�n del color.</param>
		/// <param name="depthAttachment">Informaci�n del depth / stencil.</param>
		/// <param name="msaa">Samples del MSAA.</param>
		void CreateRenderpass(VkSampleCountFlagBits msaa = VK_SAMPLE_COUNT_1_BIT);

		/// <summary>
		/// Establece el tama�o de la imagen renderizada.
		/// </summary>
		/// <param name="sizeX">Tama�o en X.</param>
		/// <param name="sizeY">Tama�o en Y.</param>
		/// <param name="createColorImage">True si (re)crea las im�genes.</param>
		void SetSize(uint32_t sizeX, uint32_t sizeY);

		void Resize(uint32_t sizeX, uint32_t sizeY);

		/// <summary>
		/// Crea el sprite para renderizar la imagen final como sprite.
		/// No es estrictamente necesario.
		/// </summary>
		/// <param name="content">Content manager que va a almacenar el sprite.</param>
		void CreateSprite(ContentManager* content);

		/// <summary>
		/// Prepara el render target para renderizar sobre �l.
		/// </summary>
		/// <param name="cmdBuffer">Command buffer.</param>
		/// <param name="layout">Layout (color por defecto).</param>
		void TransitionToRenderTarget(VkCommandBuffer cmdBuffer);

		/// <summary>
		/// Prepara el render target para ser usado como sprite / textura.
		/// </summary>
		/// <param name="cmdBuffer">Command buffer.</param>
		void TransitionToTexture(VkCommandBuffer cmdBuffer);

		/// <summary>
		/// Resetea el render target.
		/// </summary>
		/// <param name="complete">Tambi�n elimina el renderpass.</param>
		void Clear();

		void BeginRenderpass(VkCommandBuffer commandBuffer, uint32_t iteration);
		void EndRenderpass();

		/// <summary>
		/// Sprite que contiene la textura renderizada.
		/// </summary>
		Sprite renderedSprite{};

		/// <summary>
		/// Tama�o de la imagen renderizada.
		/// </summary>
		Vector2ui GetSize();

		Color clearColor = Color(0.8f, 0.8f, 0.8f);

		RenderTargetImageTarget targetImage = RenderTargetImageTarget::COLOR;

	private:

		void SetSwapchain(std::vector<VkImageView>& swapchainViews);
		std::vector<VkImageView>* swapchainViews = nullptr;

		void CreateFramebuffers(uint32_t numFb);

		VULKAN::GpuImage intermediateColorImage;
		VULKAN::GpuImage depthImage;

		std::vector<VULKAN::Framebuffer*> framebuffers;
		UniquePtr<VULKAN::Renderpass> renderpass;

		Vector2ui size;

		RenderAPI* renderer = nullptr;

		ContentManager* content = nullptr;

		/// <summary>
		/// Crea un render target vac�o.
		/// </summary>
		/// <param name="renderer">Renderizador.</param>
		RenderTarget(RenderAPI* renderer);

		/// <summary>
		/// True si el sprite puede usarse.
		/// </summary>
		bool spriteHasBeenCreated = false;

		enum {
			OSK_IMAGE_STATE_TEXTURE,
			OSK_IMAGE_STATE_RENDERTARGET
		} imageState = OSK_IMAGE_STATE_TEXTURE;

		VkCommandBuffer lastCommandBuffer = VK_NULL_HANDLE;

	};

}
