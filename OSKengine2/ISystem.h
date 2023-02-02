#pragma once

#include "OSKmacros.h"
#include "GameObject.h"
#include "Component.h"
#include "DynamicArray.hpp"

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ECS{
		
	/// @brief Un sistema se encarga de ejecutar lógica con los datos de los componentes.
	/// Los componentes no son más que colecciones de datos: la lógica está en
	/// los sistemas.
	/// 
	/// @note Cada sistema tiene un Signature, que indica qué componentes debe tener un GameObject
	/// para ser procesado por el sistema.
	/// @warning Para que una clase pueda ser usada como sistema, debe tener implementado OSK_SYSTEM(className).
	/// Este debe ser un identificador único: no pueden haber dos tipos de sistemas con el mismo className.
	class OSKAPI_CALL ISystem {

	public:

		virtual ~ISystem() = default;

		/// @brief Función que se ejecuta al crearse el sistema.
		virtual void OnCreate();

		/// @brief Función que se ejecuta al destruirse el sistema.
		virtual void OnRemove();


		/// @brief Función que se ejecuta una vez por frame.
		/// La lógica en la mayoría de sistemas estará implementada
		/// en este método.
		/// @param deltaTime Número de segundos que han pasado desde
		/// la anterior ejecución.
		virtual void OnTick(TDeltaTime deltaTime);

	protected:

		ISystem() = default;

	};


	template<typename TSystem>
	concept IsEcsSystem = requires (TSystem) {
			{TSystem::GetSystemName()};
	};

}

#ifndef OSK_SYSTEM
#define OSK_SYSTEM(className) const static inline std::string GetSystemName() { return className; }
#endif
