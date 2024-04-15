#pragma once

#include "OSKmacros.h"
#include "GameObject.h"
#include "Component.h"
#include "DynamicArray.hpp"

#include <string>
#include <set>
#include "HashMap.hpp"

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ECS{

	/// @brief Dependencias expl�citas de un sistema.
	struct SystemDependencies {

		static SystemDependencies Empty() {
			return SystemDependencies{};
		}

		/// @brief Sistemas que deben ejecutarse antes del sistema.
		std::set<std::string, std::less<>> executeAfterThese;

		/// @brief Sistemas que deben ejecutarse despu�s del sistema.
		std::set<std::string, std::less<>> executeBeforeThese;

		/// @brief Indica si este sistema debe ser ejecutado sin que otros 
		/// sistemas puedan ejecutarse al mismo tiempo.
		bool exclusiveExecution = false;

		/// @brief Indica si este sistema debe ser ejecutado exclusivamente
		/// en un �nico hilo.
		/// 
		/// Permite que otros sistemas puedan ejecutarse
		/// al mismo tiempo.
		bool singleThreaded = false;

	};


	/// @brief Un sistema se encarga de ejecutar l�gica.
	/// @warning Para que una clase pueda ser usada como sistema, debe tener implementado OSK_SYSTEM(className).
	/// Este debe ser un identificador �nico: no pueden haber dos tipos de sistemas con el mismo className.
	class OSKAPI_CALL ISystem {

	public:

		OSK_DEFINE_AS(ISystem)

		virtual ~ISystem() = default;


		/// @brief Funci�n que se ejecuta al crearse el sistema.
		virtual void OnCreate();

		/// @brief Funci�n que se ejecuta al destruirse el sistema.
		virtual void OnRemove();


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


		void _SetDependencies(const SystemDependencies& dependencies);
		const SystemDependencies& GetDependencies() const;

		virtual std::string_view GetName() const = 0;

	protected:

		ISystem() = default;

	private:

		/// @brief Representa si un sistema debe ejecutarse o no.
		bool m_isActive = true;

		SystemDependencies m_dependencies = SystemDependencies::Empty();

	};


	/// @brief Concepto que nos permite detectar si una clase cumple con las condiciones
	/// para ser un sistema.
	template<typename TSystem>
	concept IsEcsSystem = requires (TSystem) {
		{ TSystem::GetSystemName() };
	};

}

#ifndef OSK_SYSTEM
#define OSK_SYSTEM(className) constexpr static inline std::string_view GetSystemName() { return className; } \
std::string_view GetName() const override { return className; }
#endif
