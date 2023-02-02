#pragma once

#include "ISystem.h"
#include <type_traits>

namespace OSK::ECS {

	/// @brief Un sistema que aplica su funcionalidad
	/// iterando una o más veces sobre las entidades
	/// compatibles.
	class OSKAPI_CALL IIteratorSystem : public ISystem {

	public:

		virtual ~IIteratorSystem() = default;


		/// @brief Función que se ejecuta cuando un GameObject entra en la lista
		/// de objetos procesables por el sistema.
		/// @param obj 
		virtual void OnObjectAdded(GameObjectIndex obj);

		/// @brief Función que se ejecuta cuando un GameObject sale de la lista
		/// de objetos procesables por el sistema.
		/// @param obj 
		virtual void OnObjectRemoved(GameObjectIndex obj);


		/// @brief Devuelve el signature del sistema, indicando qué componentes
		/// necesita.
		/// @return Signature del sistema.
		const Signature& GetSignature() const;

		/// @brief Añade un objeto a la lista de objetos procesables.
		/// @param obj 
		void _AddObject(GameObjectIndex obj);

		/// @brief Elimina un objeto de la lista de objetos procesables.
		/// @param obj 
		void _RemoveObject(GameObjectIndex obj);

	protected:
		
		void _SetSignature(const Signature& signature);

		const DynamicArray<GameObjectIndex>& GetObjects();

	private:

		DynamicArray<GameObjectIndex> processedObjects;
		Signature signature;

	};


	/// @brief Concepto que permite saber si una clase
	/// corresponde a un sistema iterador.
	template<typename TSystem>
	concept IsIteratorSystem = std::is_base_of_v<IIteratorSystem, TSystem>
		&& IsEcsSystem<TSystem>;

}
