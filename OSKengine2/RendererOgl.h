#pragma once

#include "OSKmacros.h"
#include "IRenderer.h"

namespace OSK {

	/// <summary>
	/// Implementación de la interfaz para el renderizador de OpenGL.
	/// </summary>
	class OSKAPI_CALL RendererOgl : public IRenderer {

	public:

		void Initialize(const std::string& appName, const Version& version, const Window& window) override;

		void Close() override;
		void PresentFrame() override;

	protected:

		void CreateCommandQueues() override;
		void CreateSwapchain() override;
		void CreateSyncDevice() override;
		void CreateGpuMemoryAllocator() override;

	private:

	};

}
