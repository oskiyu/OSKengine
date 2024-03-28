#pragma once

#include "ISystem.h"
#include <type_traits>
#include <span>


namespace OSK::ECS {

	/// @brief Un sistema que aplica su funcionalidad
	/// iterando una o más veces sobre las entidades
	/// compatibles.
	class OSKAPI_CALL IIteratorSystem : public ISystem {

	public:

		virtual ~IIteratorSystem() = default;


		/// @brief Función que se ejecuta cuando un GameObject entra en la lista
		/// de objetos procesables por el sistema.
		/// @param obj Objeto añadido.
		virtual void OnObjectAdded(GameObjectIndex obj);

		/// @brief Función que se ejecuta cuando un GameObject sale de la lista
		/// de objetos procesables por el sistema.
		/// @param obj Objeto eliminado.
		virtual void OnObjectRemoved(GameObjectIndex obj);


		/// @brief Función que se ejecuta antes de comenzar la ejecución.
		virtual void OnExecutionStart();

		/// @brief Ejecuta la lógica del sistema.
		/// @param deltaTime Número de segundos que han pasado desde
		/// la anterior ejecución.
		/// @param objects Objetos a procesar.
		virtual void Execute(
			TDeltaTime deltaTime,
			std::span<const GameObjectIndex> objects);

		/// @brief Función que se ejecuta después de terminar la ejecución.
		virtual void OnExecutionEnd();

		/// @brief Devuelve el signature del sistema, indicando qué componentes
		/// necesita.
		/// @return Signature del sistema.
		const Signature& GetSignature() const;


		/// @brief Establece la lista de objetos compatibles.
		/// @param objects Objetos compatibles.
		void SetCompatibleObjects(std::span<const ECS::GameObjectIndex> objects);

		/// @return Objetos compatibles con el sistema.
		std::span<const ECS::GameObjectIndex> GetAllCompatibleObjects() const;

	protected:
		
		void _SetSignature(const Signature& signature);

	private:

		Signature m_signature;
		std::span<const ECS::GameObjectIndex> m_allCompatibleObjects;

	};


	/// @brief Concepto que permite saber si una clase
	/// corresponde a un sistema iterador.
	template<typename TSystem>
	concept IsIteratorSystem = std::is_base_of_v<IIteratorSystem, TSystem>
		&& IsEcsSystem<TSystem>;

}
