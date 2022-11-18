#pragma once

#include "ISystem.h"

#include "Vector2.hpp"
#include "UniquePtr.hpp"
#include "SharedPtr.hpp"
#include "RenderTarget.h"

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ECS {
		
	/// @brief 
	/// Clase base para sistemas de renderizado.
	/// 
	/// Cada sistema de renderizado renderizará en una imagen propia
	/// (IRenderSystem::GetSystemTargetImage).
	class OSKAPI_CALL IRenderSystem : public ISystem {

	public:

		virtual ~IRenderSystem() = default;

		virtual void OnCreate() override;

		/// @brief Comando específico del sistema, para ejecutar el renderizado.
		/// @pre La lista de comandos debe estar abierta.
		virtual void Render(GRAPHICS::ICommandList* commandList) = 0;

		/// @brief Cambia la resolución de la imagen de render target.
		/// 
		/// @param resolution Nueva resolución de renderizado.
		/// 
		/// @note Se llama cada vez que la ventana cambia de tamaño.
		/// @note Se puede sobreescribir para cambiar de tamaño otras
		/// imágenes de renderizado.
		virtual void Resize(const Vector2ui& resolution);

		/// @brief Crea la imagen de renderizado.
		/// 
		/// Si el render system va a usar más imágenes de renderizado,
		/// se deben inicializar sobreescribiendo esta función.
		/// 
		/// @param size Resolución de la imagen de renderizado.
		virtual void CreateTargetImage(const Vector2ui& size);

		/// @return Render target principal sobre el que se renderiza
		/// el sistema.
		GRAPHICS::RenderTarget& GetRenderTarget();

	protected:

		/// <summary>
		/// Configura el viewport que se usará en la lista de comandos
		/// para que incluya toda la imagen del render system.
		/// </summary>
		/// 
		/// @pre commandList no debe ser null.
		void SetupViewport(GRAPHICS::ICommandList* commandList);

		GRAPHICS::RenderTarget renderTarget;

	};

}
