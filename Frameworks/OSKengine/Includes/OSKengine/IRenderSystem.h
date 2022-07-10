#pragma once

#include "ISystem.h"

#include "Vector2.hpp"
#include "UniquePtr.hpp"
#include "SharedPtr.hpp"
#include "IRenderpass.h"
#include "RenderTarget.h"

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ECS {

	/// <summary>
	/// Clase base para sistemas de renderizado.
	/// 
	/// Cada sistema de renderizado renderizar� en una imagen propia
	/// (IRenderSystem::GetSystemTargetImage).
	/// </summary>
	class OSKAPI_CALL IRenderSystem : public ISystem {

	public:

		/// <summary>
		/// Comando espec�fico del sistema, para ejecutar el renderizado.
		/// </summary>
		/// 
		/// @pre La lista de comandos debe estar abierta.
		virtual void Render(GRAPHICS::ICommandList* commandList) = 0;

		/// <summary>
		/// Cambia el tama�o de la imagen de render target.
		/// 
		/// Se llama cada vez que la ventana cambia de tama�o.
		/// 
		/// Se puede sobreescribir para cambiar de tama�o otras
		/// im�genes de renderizado.
		/// </summary>
		/// <param name="windowSize">Nuevo tama�o de la ventana.</param>
		virtual void Resize(const Vector2ui& windowSize);

		/// <summary>
		/// Crea la imagen de renderizado.
		/// 
		/// Si el render system va a usar m�s im�genes de renderizado,
		/// se deben inicializar sobreescribiendo esta funci�n.
		/// </summary>
		/// <param name="size">Tama�o de la imagen de renderizado.</param>
		virtual void CreateTargetImage(const Vector2ui& size);

		const GRAPHICS::RenderTarget& GetRenderTarget() const;

	protected:

		GRAPHICS::RenderTarget renderTarget;

	};

}
