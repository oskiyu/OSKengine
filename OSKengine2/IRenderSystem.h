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
	/// Cada sistema de renderizado renderizar� en una imagen propia
	/// (IRenderSystem::GetSystemTargetImage).
	class OSKAPI_CALL IRenderSystem : public ISystem {

	public:

		virtual ~IRenderSystem() = default;

		virtual void OnCreate() override;

		/// @brief Comando espec�fico del sistema, para ejecutar el renderizado.
		/// @pre La lista de comandos debe estar abierta.
		virtual void Render(GRAPHICS::ICommandList* commandList) = 0;

		/// @brief Cambia la resoluci�n de la imagen de render target.
		/// 
		/// @param resolution Nueva resoluci�n de renderizado.
		/// 
		/// @note Se llama cada vez que la ventana cambia de tama�o.
		/// @note Se puede sobreescribir para cambiar de tama�o otras
		/// im�genes de renderizado.
		virtual void Resize(const Vector2ui& resolution);

		/// @brief Crea la imagen de renderizado.
		/// 
		/// Si el render system va a usar m�s im�genes de renderizado,
		/// se deben inicializar sobreescribiendo esta funci�n.
		/// 
		/// @param size Resoluci�n de la imagen de renderizado.
		virtual void CreateTargetImage(const Vector2ui& size);

		/// @return Render target principal sobre el que se renderiza
		/// el sistema.
		GRAPHICS::RenderTarget& GetRenderTarget();

	protected:

		/// <summary>
		/// Configura el viewport que se usar� en la lista de comandos
		/// para que incluya toda la imagen del render system.
		/// </summary>
		/// 
		/// @pre commandList no debe ser null.
		void SetupViewport(GRAPHICS::ICommandList* commandList);

		GRAPHICS::RenderTarget renderTarget;

	};

}
