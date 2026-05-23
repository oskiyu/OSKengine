#pragma once

#include "ApiCall.h"
#include "ISdfRenderPass.h"

namespace OSK::GRAPHICS {

	/// @brief Pase que renderiza una imagen al swapchain que
	/// será presentado en pantalla.
	/// 
	/// Permite pasar una imagen renderizada final a pantalla.
	class OSKAPI_CALL RenderToSwapchainPass : public ISdfRenderPass {

	public:

		/// @param src UUID de la imagen que será renderizada en pantalla.
		/// @param targetImages Imágenes del swapchain.
		explicit RenderToSwapchainPass(GdrImageUuid src, const std::span<GdrImageUuid, 3> targetImages);

		void Execute(ICommandList* cmdList) override;
		std::string_view GetName() const override;

	private:

		GdrImageUuid m_src;

	};

}
