#pragma once

#include "ApiCall.h"
#include "DefineAs.h"

#include "GameObject.h"
#include "Component.h"
#include "DynamicArray.hpp"

#include <string>
#include <set>
#include "HashMap.hpp"

#include "BinaryBlock.h"

#include <json.hpp>

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ECS{

	class SavedGameObjectTranslator;


	/// @brief Dependencias explícitas de un sistema.
	struct SystemDependencies {

		static SystemDependencies Empty() {
			return SystemDependencies{};
		}

		/// @brief Sistemas que deben ejecutarse antes del sistema.
		std::set<std::string, std::less<>> executeAfterThese;

		/// @brief Sistemas que deben ejecutarse después del sistema.
		std::set<std::string, std::less<>> executeBeforeThese;

		/// @brief Indica si este sistema debe ser ejecutado sin que otros 
		/// sistemas puedan ejecutarse al mismo tiempo.
		bool exclusiveExecution = false;

		/// @brief Indica si este sistema debe ser ejecutado exclusivamente
		/// en un único hilo.
		/// 
		/// Permite que otros sistemas puedan ejecutarse
		/// al mismo tiempo.
		bool singleThreaded = false;

	};


	/// @brief Un sistema se encarga de ejecutar lógica.
	/// @warning Para que una clase pueda ser usada como sistema, debe tener implementado OSK_SYSTEM(className).
	/// Este debe ser un identificador único: no pueden haber dos tipos de sistemas con el mismo className.
	class OSKAPI_CALL ISystem {

	public:

		OSK_DEFINE_AS(ISystem)

		virtual ~ISystem() = default;


		/// @brief Función que se ejecuta al crearse el sistema.
		virtual void OnCreate();

		/// @brief Función que se ejecuta al destruirse el sistema.
		virtual void OnRemove();


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

	/// @brief Interfaz para los sistemas serializables.
	class OSKAPI_CALL ISerializableSystem {

	public:

		virtual ~ISerializableSystem() = default;

		/// @return Estructura JSON con la información relevante
		/// de la configuración del sistema en un momento dado.
		virtual nlohmann::json SaveConfiguration() const = 0;

		/// @return Estructura binaria con la información relevante
		/// de la configuración del sistema en un momento dado.
		virtual PERSISTENCE::BinaryBlock SaveBinaryConfiguration() const = 0;

		/// @brief Aplica la configuración al sistema.
		/// @param config Configuración del sistema.
		/// @param translator Tabla que permite obtener los IDs generados.
		virtual void ApplyConfiguration(
			const nlohmann::json& config,
			const SavedGameObjectTranslator& translator) = 0;

		/// @brief Aplica la configuración al sistema.
		/// @param reader Lector del bloque binario que contiene
		/// los datos del sistema.
		/// @param translator Tabla que permite obtener los IDs generados.
		virtual void ApplyConfiguration(
			PERSISTENCE::BinaryBlockReader* reader,
			const SavedGameObjectTranslator& translator) = 0;

	};


	/// @brief Concepto que nos permite detectar si una clase cumple con las condiciones
	/// para ser un sistema.
	template<typename TSystem>
	concept IsEcsSystem = requires (TSystem) {
		{ TSystem::GetSystemName() };
	};

	/// @brief Concepto para saber si un sistema es serializable.
	template<typename TSystem>
	concept IsSerializableSystem = IsEcsSystem<TSystem> && std::is_base_of_v<ISerializableSystem, TSystem>;

}

#ifndef OSK_SYSTEM
#define OSK_SYSTEM(className) constexpr static inline std::string_view GetSystemName() { return className; } \
std::string_view GetName() const override { return className; }
#endif
