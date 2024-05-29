#pragma once

#include "ApiCall.h"
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

#pragma region Signatures

		/// @brief Establece el signature del objeto.
		/// @param obj Objeto a modificar.
		/// @param signature Nuevo signature.
		/// 
		/// @throws InvalidObjectException Si el objeto indicado no existe.
		void SetSignature(GameObjectIndex obj, const Signature& signature);

		/// @return Signature del objeto.
		/// @throws InvalidObjectException Si el objeto indicado no existe.
		Signature GetSignature(GameObjectIndex obj) const;


		/// @brief Actualiza el valor de una firma de un objeto
		/// en concreto.
		/// @param obj ID del objeto.
		/// @param position Tipo de componente a actualizar.
		/// @param value Nuevo valor.
		/// 
		/// @pre El objeto @p obj debe haber sido previamente creado.
		void UpdateSignature(GameObjectIndex obj, ComponentType position, bool value);

		/// @brief Actualiza el valor de una firma de un objeto
		/// en concreto, para indicar que posee un tipo de componente.
		/// @param obj ID del objeto.
		/// @param position Tipo de componente añadido.
		void AddComponent(GameObjectIndex obj, ComponentType position);

		/// @brief Actualiza el valor de una firma de un objeto
		/// en concreto, para indicar que no posee un tipo de componente.
		/// @param obj ID del objeto.
		/// @param position Tipo de componente eliminado.
		void RemoveComponent(GameObjectIndex obj, ComponentType position);


		/// @brief Comprueba si un objeto está activo.
		/// @param obj ID del objeto.
		/// @return True si está activo, false si se eliminó.
		bool IsGameObjectAlive(GameObjectIndex obj) const;

#pragma endregion

		/// @return Todos los objetos activos.
		std::span<const GameObjectIndex> GetAllLivingObjects() const;

	private:

		/// @brief Ids de objetos que han sido eliminados.
		/// Estos ids se pueden reutilizar.
		DynamicArray<GameObjectIndex> m_freeObjectIndices;


		/// @brief Signatures de todos los objetos.
		std::unordered_map<GameObjectIndex, Signature> m_signatures;

		/// @brief IDs de todos los objetos.
		DynamicArray<GameObjectIndex> m_livingObjects;

		
		/// @brief Identificador del próximo objeto (si no hay identificadores
		/// libres que se puedan reutilizar).
		UIndex64 m_nextIndex = 1;

	};

}
