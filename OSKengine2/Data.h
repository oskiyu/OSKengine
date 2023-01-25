// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#pragma once

#include "IDataElement.h"
#include "UniquePtr.hpp"
#include "HashMap.hpp"

namespace OSK::PERSISTENCE {

	/// @brief Nodo del árbol de persistencia.
	/// Puede contener datos en forma de FieldWrappers y
	/// otros nodos ahijados.
	class OSKAPI_CALL DataNode : public IDataElement {

	public:

		/// @brief Crea un nodo vacío.
		DataNode();

		/// @brief Establece el valor de un elemento del nodo.
		/// 
		/// @tparam T Tipo de dato.
		/// @param name Nombre del elemento dentro del nodo.
		/// @param value Valor del elemento.
		/// 
		/// @pre T debe ser una clase que derive de IDataElement.
		/// @note Si el elemento ya existía, se actualiza el valor,
		/// aunque sea de otro tipo.
		template <typename T>
		void SetValue(const std::string& name, const T& value) 
		requires std::is_base_of_v<IDataElement, T> {
			fields.Insert(name, new T(value));
		}

		/// @brief Devuelve el valor almacenado con el nombre dado
		/// en este nodo.
		/// @param name Nombre del valor en el nodo.
		/// @return Valor del nodo.
		/// 
		/// @pre Debe existir un valor con el nombre dado dentro de este nodo.
		/// @throws std::runtime_exception si se incumple la precondición.
		/// @warning No realiza una búsqueda recursiva en los nodos hijos.
		const IDataElement& GetField(const std::string& name) const;

		/// @brief Comprueba si este nodo tiene un valor con el nombre dado.
		/// @param name Nombre del valor.
		/// @return True si contiene un valor cualquiera con el nombre dado.
		/// 
		/// @warning No realiza una búsqueda recursiva en los nodos hijos.
		bool HasElement(const std::string& name) const;

	private:

		HashMap<std::string, UniquePtr<IDataElement>> fields;

	};

}
