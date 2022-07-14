#pragma once

#include "OSKmacros.h"
#include "HashMap.hpp"
#include "UniquePtr.hpp"
#include "GameObject.h"
#include "Component.h"

#include <string>

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ECS {

	class ISystem;

	/// <summary>
	/// El SystemManager se encarga de almacenar y manejar los sistemas del juego.
	/// Es el encargado de llamar a las funciones OnTick de los sistemas, además
	/// de introducir y eliminar de los sistemas a los GameObjects que cada sistema
	/// vaya a procesar.
	/// 
	/// @note Dueño de los sistemas.
	/// </summary>
	class OSKAPI_CALL SystemManager {

	public:

		/// <summary>
		/// Ejecuta la lógica OnTickd de todos los sistemas.
		/// </summary>
		void OnTick(TDeltaTime deltaTime);

		/// <summary>
		/// Elimina el GameObject de todos los sistemas.
		/// </summary>
		void GameObjectDestroyed(GameObjectIndex obj);

		/// <summary>
		/// Añade o elimina el objeto de los sistemas, según sea conveniente
		/// dependiendo del nuevo signature.
		/// </summary>
		/// <param name="obj">Objeto cuyo signature ha cambiado.</param>
		/// <param name="signature">Nuevo signature del objeto.</param>
		void GameObjectSignatureChanged(GameObjectIndex obj, const Signature& signature);

		/// <summary>
		/// Crea y almacena un nuevo sistema.
		/// </summary>
		template <typename TSystem> TSystem* RegisterSystem() {
			TSystem* sistema = new TSystem;

			sistemas.Insert(TSystem::GetSystemName(), (ISystem*)sistema);

			return sistema;
		}

		/// <summary>
		/// Elimina el sistema dado, para que no sea procesado a partir de ahora.
		/// </summary>
		/// 
		/// @note Si el sistema no está registrado, no ocurre nada.
		/// 
		/// @bug No llama a ISystem::OnRemove().
		template <typename TSystem> void RemoveSystem() {
			sistemas.Remove(TSystem::GetSystemName());
		}

		/// <summary>
		/// Devuelve la instancia del sistema dado.
		/// </summary>
		template <typename TSystem> TSystem* GetSystem() const {
			return (TSystem*)sistemas.Get(TSystem::GetSystemName()).GetPointer();
		}

		/// <summary>
		/// Comprueba si un sistema dado está registrado.
		/// </summary>
		template <typename TSystem> bool ContainsSystem() const {
			return sistemas.ContainsKey(TSystem::GetSystemName());
		}

	private:

		HashMap<std::string, UniquePtr<ISystem>> sistemas;

	};

}
