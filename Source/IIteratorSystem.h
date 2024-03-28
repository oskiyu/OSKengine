#pragma once

#include "ISystem.h"
#include <type_traits>
#include <span>


namespace OSK::ECS {

	/// @brief Un sistema que aplica su funcionalidad
	/// iterando una o m�s veces sobre las entidades
	/// compatibles.
	class OSKAPI_CALL IIteratorSystem : public ISystem {

	public:

		virtual ~IIteratorSystem() = default;


		/// @brief Funci�n que se ejecuta cuando un GameObject entra en la lista
		/// de objetos procesables por el sistema.
		/// @param obj Objeto a�adido.
		virtual void OnObjectAdded(GameObjectIndex obj);

		/// @brief Funci�n que se ejecuta cuando un GameObject sale de la lista
		/// de objetos procesables por el sistema.
		/// @param obj Objeto eliminado.
		virtual void OnObjectRemoved(GameObjectIndex obj);


		/// @brief Funci�n que se ejecuta antes de comenzar la ejecuci�n.
		virtual void OnExecutionStart();

		/// @brief Ejecuta la l�gica del sistema.
		/// @param deltaTime N�mero de segundos que han pasado desde
		/// la anterior ejecuci�n.
		/// @param objects Objetos a procesar.
		virtual void Execute(
			TDeltaTime deltaTime,
			std::span<const GameObjectIndex> objects);

		/// @brief Funci�n que se ejecuta despu�s de terminar la ejecuci�n.
		virtual void OnExecutionEnd();

		/// @brief Devuelve el signature del sistema, indicando qu� componentes
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
