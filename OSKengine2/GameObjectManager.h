#pragma once

#include "OSKmacros.h"
#include "GameObject.h"
#include "DynamicArray.hpp"
#include "Component.h"

namespace OSK::ECS {

	/// <summary>
	/// Se encarga de registrar y eliminar GameObjects.
	/// </summary>
	class OSKAPI_CALL GameObjectManager {

	public:

		/// <summary>
		/// Registra un nuevo objeto.
		/// </summary>
		/// <returns>Id del objeto.</returns>
		GameObjectIndex CreateGameObject();

		/// <summary>
		/// Destruye el objeto dado.
		/// 
		/// @warning El identificador del objeto se pondrá a 0, para
		/// indicar que es un identificador inválido.
		/// </summary>
		/// <param name="obj"></param>
		/// 
		/// @throws InvalidObjectException Si el objeto indicado no existe.
		void DestroyGameObject(GameObjectIndex* obj);

		/// <summary>
		/// Establece el signature del objeto.
		/// </summary>
		/// 
		/// @throws InvalidObjectException Si el objeto indicado no existe.
		void SetSignature(GameObjectIndex obj, const Signature& signature);

		/// <summary>
		/// Devuelve el signature del objeto.
		/// </summary>
		/// 
		/// @throws InvalidObjectException Si el objeto indicado no existe.
		Signature GetSignature(GameObjectIndex obj) const;

		/// @brief Comprueba si un objeto está activo.
		/// @param obj ID del objeto.
		/// @return True si está activo, false si se eliminó.
		bool IsGameObjectAlive(GameObjectIndex obj) const;

	private:

		/// <summary>
		/// Ids de objetos que han sido eliminados.
		/// Estos ids se pueden reutilizar.
		/// </summary>
		DynamicArray<GameObjectIndex> freeObjectIndices;

		/// <summary>
		/// Signatures de todos los objetos.
		/// 
		/// signatures[objectId] = signature del objeto.
		/// </summary>
		DynamicArray<Signature> signatures;

		/// <summary>
		/// Identificador del próximo objeto (si no hay identificadores
		/// libres que se puedan reutilizar).
		/// </summary>
		GameObjectIndex nextIndex = 1;

	};

}
