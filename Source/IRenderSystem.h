#pragma once

#include "IPureSystem.h"

#include "Vector2.hpp"
#include "UniquePtr.hpp"
#include "SharedPtr.hpp"
#include "RenderTarget.h"

#include "IRenderPass.h"

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ECS {
		
	/// @brief 
	/// Clase base para sistemas de renderizado.
	/// 
	/// Cada sistema de renderizado renderizará en una imagen propia
	/// (IRenderSystem::GetSystemTargetImage).
	class OSKAPI_CALL IRenderSystem : public IPureSystem {

	public:

		virtual ~IRenderSystem();

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


		// -- PASES -- //

		/// @brief Añade un nuevo pase de renderizado.
		/// @param pass Pase de renderizado.
		/// @pre No debe existir un pase de renderizado previo con el mismo nombre
		/// que @p pass.
		virtual void AddRenderPass(OwnedPtr<GRAPHICS::IRenderPass> pass);

		/// @param name Nombre del pase de renderizado.
		/// @return True si contiene un pase de renderizado con el nombre @p name.
		bool HasRenderPass(std::string_view name) const;

		/// @param name Nombre del pase de renderizado.
		/// @return Pase de renderizado.
		/// @pre Debe existir un pase de renderizado con nombre @p name.
		GRAPHICS::IRenderPass* GetRenderPass(std::string_view name);

		/// @brief Actualiza las listas de objetos asignados a cada pase.
		virtual void UpdatePerPassObjectLists();

	protected:

		/// @brief Configura el viewport que se usará en la lista de comandos
		/// para que incluya toda la imagen del render system.
		/// @param commandList Lista de comandos.
		/// @pre commandList no debe ser null.
		void SetupViewport(GRAPHICS::ICommandList* commandList);

		// -- PASES -- //

		/// @brief Pases de renderizado.
		DynamicArray<UniquePtr<GRAPHICS::IRenderPass>> m_renderPasses;

		/// @brief Mapa nombre -> pase de renderizado.
		std::unordered_map<std::string, GRAPHICS::IRenderPass*, StringHasher, std::equal_to<>> m_renderPassesTable;

		/// @brief Objetos compatibles con cada pase.
		std::unordered_map<std::string, DynamicArray<GameObjectIndex>, StringHasher, std::equal_to<>> m_objectsPerPass;


		GRAPHICS::RenderTarget m_renderTarget;

	};

}
