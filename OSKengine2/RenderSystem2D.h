#pragma once

#include "ISystem.h"

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ECS {

	class OSKAPI_CALL RenderSystem2D : public ISystem {

	public:

		OSK_SYSTEM("OSK::RenderSystem2D");

		RenderSystem2D();

		/// <summary>
		/// Comando específico del sistema, para ejecutar el renderizado.
		/// </summary>
		void Render(GRAPHICS::ICommandList* commandList);

	};

}
