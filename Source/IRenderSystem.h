#pragma once

#include "IIteratorSystem.h"

#include "Vector2.hpp"
#include "UniquePtr.hpp"
#include "SharedPtr.hpp"
#include "RenderTarget.h"

#include "IShaderPass.h"
#include "MeshMapping.h"


namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ECS {
		

	/// @brief 
	/// Clase base para sistemas de renderizado.
	/// 
	/// Cada sistema de renderizado renderizar� en una imagen propia
	/// (IRenderSystem::GetSystemTargetImage).
	class OSKAPI_CALL IRenderSystem : public IIteratorSystem {

	public:

		virtual ~IRenderSystem();

		virtual void OnCreate() override;

		/// @brief Funci�n que se ejecuta antes de comenzar el proceso de objetos.
		/// @param commandList Lista de comandos.
		virtual void OnRenderStart(GRAPHICS::ICommandList* commandList);

		virtual void Execute(
			TDeltaTime deltaTime,
			std::span<const GameObjectIndex> objects) override;

		/// @brief Comando espec�fico del sistema, para ejecutar el renderizado.
		/// @pre La lista de comandos debe estar abierta.
		virtual void Render(
			GRAPHICS::ICommandList* commandList, 
			std::span<const ECS::GameObjectIndex> objects) = 0;

		/// @brief Funci�n que se ejecuta despu�s de finalizar el proceso de objetos.
		/// @param commandList Lista de comandos.
		virtual void OnRenderEnd(GRAPHICS::ICommandList* commandList);


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


		// -- PASES -- //

		/// @brief A�ade un nuevo pase de renderizado.
		/// @param pass Pase de renderizado.
		/// @pre No debe existir un pase de renderizado previo con el mismo nombre
		/// que @p pass.
		virtual void AddShaderPass(UniquePtr<GRAPHICS::IShaderPass>&& pass);

		/// @brief A�ade un nuevo pase de renderizado de sombras.
		/// @param pass Pase de renderizado de sombras.
		/// @pre No debe existir un pase de renderizado de sombras previo con el mismo nombre
		/// que @p pass.
		virtual void AddShadowsPass(UniquePtr<GRAPHICS::IShaderPass>&& pass);

		GRAPHICS::IShaderPass* GetShaderPass(std::string_view name);
		const GRAPHICS::IShaderPass* GetShaderPass(std::string_view name) const;


		/// @brief Actualiza las listas de objetos asignados a cada pase.
		virtual void UpdatePerPassObjectLists(std::span<const ECS::GameObjectIndex> objects);


		/// @return Todos los pases de renderizado normal.
		std::span<GRAPHICS::IShaderPass* const> GetAllShaderPasses() const;

		/// @return Todos los pases de renderizado de sombras.
		std::span<GRAPHICS::IShaderPass* const> GetAllShadowsPasses() const;

	protected:

		void UpdatePassesCamera(ECS::GameObjectIndex cameraObject);

		/// @brief Configura el viewport que se usar� en la lista de comandos
		/// para que incluya toda la imagen del render system.
		/// @param commandList Lista de comandos.
		/// @pre commandList no debe ser null.
		void SetupViewport(GRAPHICS::ICommandList* commandList);


		// -- PASES -- //

		GRAPHICS::ShaderPassTable m_shaderPasses{};
		GRAPHICS::ShaderPassTable m_shadowsPasses{};


		GRAPHICS::RenderTarget m_renderTarget;

		GRAPHICS::GlobalMeshMapping m_meshMapping = {};

	};

}
