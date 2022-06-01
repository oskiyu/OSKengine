#pragma once

#include "SpriteRenderer.h"

namespace OSK::GRAPHICS { 
	class ICommandList;
}
namespace OSK::ECS {
	class Transform2D;
}

namespace OSK::UI {

	class UiElement;

	/// <summary>
	/// Clase que ayuda a renderizar una interfaz de usuario.
	/// </summary>
	class OSKAPI_CALL UiRenderer {

	public:

		/// <summary>
		/// Renderiza un elemento de UI y sus hijos.
		/// </summary>
		/// 
		/// @pre La cola de comandos debe estar iniciada.
		/// @pre La cola de comandos debe tener asociado un renderpass.
		void Render(GRAPHICS::ICommandList* commandList, const UiElement* element);

	private:

		/// <summary>
		/// Renderiza un elemento de UI y sus hijos.
		/// </summary>
		/// 
		/// @pre La cola de comandos debe estar iniciada.
		/// @pre La cola de comandos debe tener asociado un renderpass.
		void RenderElement(const UiElement* element, const ECS::Transform2D& transform);

		GRAPHICS::SpriteRenderer spriteRenderer;

	};

}
