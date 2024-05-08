#pragma once

#include "OSKmacros.h"
#include "HashMap.hpp"
#include "UniquePtr.hpp"
#include "ISystem.h"

#include <span>


namespace OSK::ECS {

	/// @brief Set de sistemas que pueden ejecutarse en paralelo,
	/// al no tener dependencias entre ellos.
	struct SystemExecutionSet {

		/// @brief Sistemas del set.
		DynamicArray<ISystem*> systems;

	};


	/// @brief Grafo que contiene los sistemas ordenados de tal manera
	/// que se respeten sus dependencias.
	class OSKAPI_CALL SystemExecutionGraph {

	public:

		/// @brief Añade el sistema al grafo de ejecución.
		/// @param system Sistema a añadir.
		/// @param dependencies Dependencias del sistema.
		/// 
		/// @pre No debe haber un sistema del tipo de @p system previamente registrado.
		/// @pre No debe haber ninguna dependencia cíclica al incluir el sistema
		/// @p system.
		/// 
		/// @throws SystemCyclicDependency si hay una dependencia cíclica.
		void AddSystem(ISystem* system, const SystemDependencies& dependencies);

		/// @brief Quita un sistema del grafo de ejecución.
		/// @param system Sistema a eliminar.
		void RemoveSystem(const ISystem* system);

		/// @return Sets de ejecución, ordenados.
		std::span<SystemExecutionSet> GetExecutionGraph();

	private:

		/// @brief Reconstryue el grafo de ejecución.
		void Rebuild();

		/// @return True si no existe ninguna dependencia cíclica en el grafo.
		bool Validate() const;


		/// @brief Comprueba que no exista una dependencia cíclica hacia delante,
		/// comprobando las dependencias executeBeforeThese.
		/// @param startPoint Sistema inicial (hay dependencia cíclica si volvemos a ese sistema).
		/// @param system Sistema a comprobar.
		/// Se comprueba este y sus dependencias executeBeforeThese.
		/// @return True si no hay dependencia cíclica, false en caso contrario.
		bool ValidatePreviousDependencies(std::string_view startPoint, const ISystem* system) const;

		/// @brief Comprueba que no exista una dependencia cíclica hacia detrás,
		/// comprobando las dependencias executeAfterThese.
		/// @param startPoint Sistema inicial (hay dependencia cíclica si volvemos a ese sistema).
		/// @param system Sistema a comprobar.
		/// Se comprueba este y sus dependencias executeAfterThese.
		/// @return True si no hay dependencia cíclica, false en caso contrario.
		bool ValidateFollowingDependencies(std::string_view startPoint, const ISystem* system) const;


		/// @brief Grafo representado de manera lineal.
		DynamicArray<SystemExecutionSet> m_flatExecutionGraph;

		std::unordered_map<std::string, ISystem*, StringHasher, std::equal_to<>> m_allRegisteredSystems;

	};

}
