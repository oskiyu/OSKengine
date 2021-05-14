#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>

#include "Renderpass.h"
#include "GPUImage.h"

namespace OSK {

	class RenderAPI;

}

namespace OSK::VULKAN {

	/// <summary>
	/// El framebuffer se usa para renderizar una escena en una imagen.
	/// Establece qu� imagen se va a usar como target a la hora de renderizar.
	/// </summary>
	class OSKAPI_CALL Framebuffer {

		friend class OSK::RenderAPI;
		friend class RenderizableScene;
		friend class RenderSystem3D;

	public:

		/// <summary>
		/// Destruye el framebuffer.
		/// </summary>
		~Framebuffer();

		/// <summary>
		/// A�ade una imagen sobre la que se va a renderizar.
		/// </summary>
		/// <param name="image">Imagen.</param>
		void AddImageView(VULKAN::GPUImage* image);

		/// <summary>
		/// A�ade una imagen sobre la que se va a renderizar.
		/// </summary>
		/// <param name="view">View de la imagen.</param>
		void AddImageView(VkImageView view);

		/// <summary>
		/// Crea el framebuffer.
		/// LLamar despu�s de establecer las im�genes.
		/// </summary>
		/// <param name="renderpass">Renderpass que va a renderizar sobre la imagen.</param>
		/// <param name="sizeX">Tama�o del framebuffer.</param>
		/// <param name="sizeY">Tama�o del framebuffer.</param>
		void Create(VULKAN::Renderpass* renderpass, uint32_t sizeX, uint32_t sizeY);

		/// <summary>
		/// Elimina el framebuffer.
		/// </summary>
		void Clear();

	private:

		/// <summary>
		/// Crea un framebuffer vac�o.
		/// </summary>
		/// <param name="renderer">Renderizador.</param>
		Framebuffer(OSK::RenderAPI* renderer);

		/// <summary>
		/// Framebuffer nativo.
		/// </summary>
		VkFramebuffer framebuffer;

		/// <summary>
		/// Renderizador.
		/// </summary>
		OSK::RenderAPI* renderer = nullptr;

		/// <summary>
		/// Im�genes del framebuffer.
		/// </summary>
		std::vector<VkImageView> attachments;

		/// <summary>
		/// Tama�o del framebuffer.
		/// </summary>
		Vector2ui size;

		/// <summary>
		/// True si el framebuffer puede utilizarse.
		/// </summary>
		bool isActive = false;

	};

}