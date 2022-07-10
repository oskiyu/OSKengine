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
	/// Cada sistema de renderizado renderizará en una imagen propia
	/// (IRenderSystem::GetSystemTargetImage).
	/// </summary>
	class OSKAPI_CALL IRenderSystem : public ISystem {

	public:

		/// <summary>
		/// Comando específico del sistema, para ejecutar el renderizado.
		/// </summary>
		/// 
		/// @pre La lista de comandos debe estar abierta.
		virtual void Render(GRAPHICS::ICommandList* commandList) = 0;

		/// <summary>
		/// Cambia el tamaño de la imagen de render target.
		/// 
		/// Se llama cada vez que la ventana cambia de tamaño.
		/// 
		/// Se puede sobreescribir para cambiar de tamaño otras
		/// imágenes de renderizado.
		/// </summary>
		/// <param name="windowSize">Nuevo tamaño de la ventana.</param>
		virtual void Resize(const Vector2ui& windowSize);

		/// <summary>
		/// Crea la imagen de renderizado.
		/// 
		/// Si el render system va a usar más imágenes de renderizado,
		/// se deben inicializar sobreescribiendo esta función.
		/// </summary>
		/// <param name="size">Tamaño de la imagen de renderizado.</param>
		virtual void CreateTargetImage(const Vector2ui& size);

		const GRAPHICS::RenderTarget& GetRenderTarget() const;

	protected:

		GRAPHICS::RenderTarget renderTarget;

	};

}
