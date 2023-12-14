#pragma once

#include "OSKmacros.h"
#include "GameObject.h"
#include "DynamicArray.hpp"
#include "HashMap.hpp"
#include "Component.h"

namespace OSK::ECS {

	/// @brief Se encarga de registrar y eliminar GameObjects.
	class OSKAPI_CALL GameObjectManager {

	public:

		/// @brief Registra un nuevo objeto.
		/// @return Id del nuevo objeto.
		GameObjectIndex CreateGameObject();

		/// @brief Destruye el objeto dado.
		/// @param obj Objeto a eliminar.
		/// 
		/// @warning El identificador del objeto se pondrá a 0, para
		/// indicar que es un identificador inválido.
		/// 
		/// @throws InvalidObjectException Si el objeto indicado no existe.
		void DestroyGameObject(GameObjectIndex* obj);


		/// @brief Establece el signature del objeto.
		/// @param obj Objeto a modificar.
		/// @param signature Nuevo signature.
		/// 
		/// @throws InvalidObjectException Si el objeto indicado no existe.
		void SetSignature(GameObjectIndex obj, const Signature& signature);

		/// @return Signature del objeto.
		/// @throws InvalidObjectException Si el objeto indicado no existe.
		Signature GetSignature(GameObjectIndex obj) const;


		/// @brief Comprueba si un objeto está activo.
		/// @param obj ID del objeto.
		/// @return True si está activo, false si se eliminó.
		bool IsGameObjectAlive(GameObjectIndex obj) const;

	private:

		/// @brief Ids de objetos que han sido eliminados.
		/// Estos ids se pueden reutilizar.
		DynamicArray<GameObjectIndex> m_freeObjectIndices;

		/// @brief Signatures de todos los objetos.
		std::unordered_map<GameObjectIndex, Signature> m_signatures;
		
		/// @brief Identificador del próximo objeto (si no hay identificadores
		/// libres que se puedan reutilizar).
		GameObjectIndex m_nextIndex = 1;

	};

}
