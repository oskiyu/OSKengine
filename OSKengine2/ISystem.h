#pragma once

#include "OSKmacros.h"
#include "GameObject.h"
#include "Component.h"
#include "DynamicArray.hpp"

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ECS{
		
	/// @brief Un sistema se encarga de ejecutar l�gica con los datos de los componentes.
	/// Los componentes no son m�s que colecciones de datos: la l�gica est� en
	/// los sistemas.
	/// 
	/// @note Cada sistema tiene un Signature, que indica qu� componentes debe tener un GameObject
	/// para ser procesado por el sistema.
	/// @warning Para que una clase pueda ser usada como sistema, debe tener implementado OSK_SYSTEM(className).
	/// Este debe ser un identificador �nico: no pueden haber dos tipos de sistemas con el mismo className.
	class OSKAPI_CALL ISystem {

	public:

		/// @brief �ndice de ordenaci�n por defecto.
		static constexpr int DEFAULT_EXECUTION_ORDER = 0;

	public:

		virtual ~ISystem() = default;

		/// @brief Funci�n que se ejecuta al crearse el sistema.
		virtual void OnCreate();

		/// @brief Funci�n que se ejecuta al destruirse el sistema.
		virtual void OnRemove();


		/// @brief Funci�n que se ejecuta una vez por frame.
		/// La l�gica en la mayor�a de sistemas estar� implementada
		/// en este m�todo.
		/// @param deltaTime N�mero de segundos que han pasado desde
		/// la anterior ejecuci�n.
		virtual void OnTick(TDeltaTime deltaTime);


		/// @brief Activa el sistema, con lo que se ejecutar�
		/// cada frame.
		void Activate();

		/// @brief Desactiva el sistema, con lo que dejar� de ejecutarse.
		void Deactivate();

		/// @brief Establece si el sistema debe ejecutarse o no.
		/// @param isActive True si debe ejecutarse, false en caso contrario.
		void SetActivationStatus(bool isActive);

		/// @brief Cambia entre activado/desactivado.
		void ToggleActivationStatus();


		/// @brief Permite saber si un sistema est� activo, es decir,
		/// si un sistema se ejcutar� cada frame o no.
		/// @return True si est� activo, false en caso contrario.
		bool IsActive() const;


		/// @return �ndice de ordenaci�n de ejecuci�n.
		int GetExecutionOrder() const;

		/// @brief Establece el nuevo �ndice de ordenaci�n de ejecuci�n.
		/// @param newOrder Nuevo �ndice.
		void _SetExecutionOrder(int newOrder);

	protected:

		ISystem() = default;

	private:

		/// @brief Representa si un sistema debe ejecutarse o no.
		bool isActive = true;

		/// @brief �ndice de ordenaci�n de ejecuci�n.
		int order = DEFAULT_EXECUTION_ORDER;

	};


	/// @brief Concepto que nos permite detectar si una clase cumple con las condiciones
	/// para ser un sistema.
	template<typename TSystem>
	concept IsEcsSystem = requires (TSystem) {
		{ TSystem::GetSystemName() };
	};


}

#ifndef OSK_SYSTEM
#define OSK_SYSTEM(className) constexpr static inline std::string_view GetSystemName() { return className; }
#endif
