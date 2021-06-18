#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Component.h"

#include <queue>
#include <array>

namespace OSK::ECS {

	/// <summary>
	/// ID de un objeto.
	/// </summary>
	typedef uint32_t GameObjectID;

	/// <summary>
	/// Máximo de objetos a guardar.
	/// </summary>
	constexpr GameObjectID MAX_OBJECTS = 10000;


	/// <summary>
	/// Clase que maneja los objetos y sus signatures.
	/// </summary>
	class OSKAPI_CALL GameObjectManager {

	public:
				
		/// <summary>
		/// Crea el manager vacío.
		/// </summary>
		GameObjectManager();

		/// <summary>
		/// Registra un nuevo objeto.
		/// </summary>
		/// <returns>ID del nuevo objeto.</returns>
		GameObjectID CreateGameObject();

		/// <summary>
		/// Elimina un objeto.
		/// </summary>
		/// <param name="object">ID del objeto.</param>
		void DestroyGameObject(GameObjectID object);

		/// <summary>
		/// Establece el signature (componentes que tiene).
		/// </summary>
		/// <param name="obj">ID del objeto.</param>
		/// <param name="sign">Componentes que ocntiene.</param>
		void SetSignature(GameObjectID obj, Signature sign);
		
		/// <summary>
		/// Obtiene los tipos de componentes que tiene.
		/// </summary>
		/// <param name="obj">ID del objeto.</param>
		/// <returns>Signature.</returns>
		Signature GetSignature(GameObjectID obj) const;

	private:

		/// <summary>
		/// IDs sin asignar.
		/// </summary>
		std::queue<GameObjectID> freeObjects;

		/// <summary>
		/// Signatures de los objetos.
		/// </summary>
		std::array<Signature, MAX_OBJECTS> signatures;

		/// <summary>
		/// Número de objetos creados.
		/// </summary>
		uint32_t livingEnitites = 0;

	};

}