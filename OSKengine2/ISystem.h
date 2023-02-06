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


		/// @brief Activa el sistema, con lo que se ejecutará
		/// cada frame.
		void Activate();

		/// @brief Desactiva el sistema, con lo que dejará de ejecutarse.
		void Deactivate();

		/// @brief Establece si el sistema debe ejecutarse o no.
		/// @param isActive True si debe ejecutarse, false en caso contrario.
		void SetActivationStatus(bool isActive);

		/// @brief Cambia entre activado/desactivado.
		void ToggleActivationStatus();


		/// @brief Permite saber si un sistema está activo, es decir,
		/// si un sistema se ejcutará cada frame o no.
		/// @return True si está activo, false en caso contrario.
		bool IsActive() const;

	protected:

		ISystem() = default;

	private:

		/// @brief Representa si un sistema debe ejecutarse o no.
		bool isActive = true;

	};


	template<typename TSystem>
	concept IsEcsSystem = requires (TSystem) {
			{TSystem::GetSystemName()};
	};

}

#ifndef OSK_SYSTEM
#define OSK_SYSTEM(className) const static inline std::string GetSystemName() { return className; }
#endif
