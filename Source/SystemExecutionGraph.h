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

		/// @brief A�ade el sistema al grafo de ejecuci�n.
		/// @param system Sistema a a�adir.
		/// @param dependencies Dependencias del sistema.
		/// 
		/// @pre No debe haber un sistema del tipo de @p system previamente registrado.
		/// @pre No debe haber ninguna dependencia c�clica al incluir el sistema
		/// @p system.
		/// 
		/// @throws SystemCyclicDependency si hay una dependencia c�clica.
		void AddSystem(ISystem* system, const SystemDependencies& dependencies);

		/// @brief Quita un sistema del grafo de ejecuci�n.
		/// @param system Sistema a eliminar.
		void RemoveSystem(const ISystem* system);

		/// @return Sets de ejecuci�n, ordenados.
		std::span<SystemExecutionSet> GetExecutionGraph();

	private:

		/// @brief Reconstryue el grafo de ejecuci�n.
		void Rebuild();

		/// @return True si no existe ninguna dependencia c�clica en el grafo.
		bool Validate() const;


		/// @brief Comprueba que no exista una dependencia c�clica hacia delante,
		/// comprobando las dependencias executeBeforeThese.
		/// @param startPoint Sistema inicial (hay dependencia c�clica si volvemos a ese sistema).
		/// @param system Sistema a comprobar.
		/// Se comprueba este y sus dependencias executeBeforeThese.
		/// @return True si no hay dependencia c�clica, false en caso contrario.
		bool ValidatePreviousDependencies(std::string_view startPoint, const ISystem* system) const;

		/// @brief Comprueba que no exista una dependencia c�clica hacia detr�s,
		/// comprobando las dependencias executeAfterThese.
		/// @param startPoint Sistema inicial (hay dependencia c�clica si volvemos a ese sistema).
		/// @param system Sistema a comprobar.
		/// Se comprueba este y sus dependencias executeAfterThese.
		/// @return True si no hay dependencia c�clica, false en caso contrario.
		bool ValidateFollowingDependencies(std::string_view startPoint, const ISystem* system) const;


		/// @brief Grafo representado de manera lineal.
		DynamicArray<SystemExecutionSet> m_flatExecutionGraph;

		std::unordered_map<std::string, ISystem*, StringHasher, std::equal_to<>> m_allRegisteredSystems;

	};

}
