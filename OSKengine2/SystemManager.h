#pragma once

#include "OSKmacros.h"
#include "HashMap.hpp"
#include "OwnedPtr.h"
#include "GameObject.h"
#include "Component.h"

#include <string>

namespace OSK {
	class ICommandList;
}

namespace OSK::ECS {

	class ISystem;

	/// <summary>
	/// El SystemManager se encarga de almacenar y manejar los sistemas del juego.
	/// Es el encargado de llamar a las funciones OnTick de los sistemas, adem�s
	/// de introducir y eliminar de los sistemas a los GameObjects que cada sistema
	/// vaya a procesar.
	/// </summary>
	class OSKAPI_CALL SystemManager {

	public:

		/// <summary>
		/// Ejecuta la l�gica OnTickd de todos los sistemas.
		/// </summary>
		void OnTick(TDeltaTime deltaTime);

		/// <summary>
		/// Elimina el GameObject de todos los sistemas.
		/// </summary>
		void GameObjectDestroyed(GameObjectIndex obj);

		/// <summary>
		/// A�ade o elimina el objeto de los sistemas, seg�n sea conveniente
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
		/// Devuelve la instancia del sistema dado.
		/// </summary>
		template <typename TSystem> TSystem* GetSystem() const {
			return (TSystem*)sistemas.Get(TSystem::GetSystemName()).GetPointer();
		}

	private:

		HashMap<std::string, OwnedPtr<ISystem>> sistemas;

	};

}