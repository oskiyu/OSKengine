#pragma once

#include "ISystem.h"

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ECS {

	/// <summary>
	/// Sistema que se encarga del renderizado de modelos 3D de los objetos.
	/// 
	/// Signature:
	/// - ModelComponent3D.
	/// - Transform3D.
	/// </summary>
	class OSKAPI_CALL RenderSystem3D : public ISystem {

	public:

		OSK_SYSTEM("OSK::RenderSystem3D");

		RenderSystem3D();

		/// <summary>
		/// Comando específico del sistema, para ejecutar el renderizado.
		/// </summary>
		/// 
		/// @pre La lista de comandos debe estar abierta.
		/// @pre La lista de comandos debe tener un renderpass activo.
		void Render(GRAPHICS::ICommandList* commandList);
		
	private:

	};

}
