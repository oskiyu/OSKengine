#pragma once

#include "IRenderSystem.h"

namespace OSK::ECS {

	class OSKAPI_CALL TerrainRenderSystem : public IPureSystem {

	public:

		OSK_SYSTEM("OSK::TerrainRenderSystem");

		TerrainRenderSystem();

		/// <summary>
		/// Comando específico del sistema, para ejecutar el renderizado.
		/// </summary>
		/// 
		/// @pre La lista de comandos debe estar abierta.
		/// @pre La lista de comandos debe tener un renderpass activo.
		void Render(GRAPHICS::ICommandList* commandList);

	};

}
