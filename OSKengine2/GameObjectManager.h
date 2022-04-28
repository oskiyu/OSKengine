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
		/// @warning El identificador del objeto se pondr� a 0, para
		/// indicar que es un identificador inv�lido.
		/// </summary>
		/// <param name="obj"></param>
		void DestroyGameObject(GameObjectIndex* obj);

		/// <summary>
		/// Establece el signature del objeto.
		/// </summary>
		void SetSignature(GameObjectIndex obj, const Signature& signature);

		/// <summary>
		/// Devuelve el signature del objeto.
		/// </summary>
		Signature GetSignature(GameObjectIndex obj);

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
		/// Identificador del pr�ximo objeto (si no hay identificadores
		/// libres que se puedan reutilizar).
		/// </summary>
		TSize nextIndex = 1;

	};

}
