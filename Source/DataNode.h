// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#pragma once

#include "HashMap.hpp"
#include "DynamicArray.hpp"
#include "Buffer.h"
#include "SchemaId.h"
#include "DataType.h"
#include "UniquePtr.hpp"

#include <string>

namespace OSK::PERSISTENCE {

	/// @brief Nodo del árbol de persistencia.
	/// Puede contener datos en forma de FieldWrappers y
	/// otros nodos ahijados.
	/// @todo Error-handling.
	class OSKAPI_CALL DataNode {

	public:

		/// @brief Crea un nodo vacío con el schema indicado.
		/// @param schemaId Schema seguido por el nodo.
		explicit DataNode(const SchemaId& schemaId);

		/// @return Schema seguido por este nodo.
		const SchemaId& GetSchemaId() const;


		/// @brief Establece el valor de una variable del nodo.
		/// La variable se colocará al final del nodo.
		/// @tparam T Tipo del dato.
		/// @param name Nombre de la variable.
		/// @param dataType Identificador del tipo de dato.
		/// @param value Valor de la variable.
		/// 
		/// @pre No debe haber previamente una variable con el nombre @p name.
		template <typename T>
		void SetValue(std::string_view name, TDataType dataType, const T& value) {
			const TFieldOffset offset = m_buffer.GetSize();
			m_buffer.AddData(value);
			m_offsetsMap[static_cast<std::string>(name)] = offset;
			m_dataTypesMap[static_cast<std::string>(name)] = dataType;
		}

		/// @brief Obtiene el valor de una variable del nodo.
		/// @tparam T Tipo de dato de la variable.
		/// @param name Nombre de la variable.
		/// @return Valor de la variable.
		/// 
		/// @pre Debe existir una variable con el nombre @p name.
		template <typename T>
		const T& GetField(std::string_view name) const {
			return m_buffer.GetData<T>(m_offsetsMap.at(static_cast<std::string>(name)));
		}


		/// @param name Nombre de una variable del nodo.
		/// @return Identificador del tipo de dato del nodo.
		TDataType GetDataType(std::string_view name) const;

		/// @param name Nombre de una variable del nodo.
		/// @return True si existe una variable del nodo con el nombre dado,
		/// falso en caso contrario.
		bool HasElement(std::string_view name) const;

		/// @return Buffer con la información almacenada,
		/// sin incluir los nodos hijos.
		const MEMORY::Buffer& GetBuffer() const;

	private:

		using TFieldOffset = UIndex64;

		SchemaId m_schemaId;
		MEMORY::Buffer m_buffer;
		std::unordered_map<std::string, TFieldOffset> m_offsetsMap;
		std::unordered_map<std::string, TDataType> m_dataTypesMap;

	};

}
