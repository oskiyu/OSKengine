// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#pragma once

#include "DynamicArray.hpp"
#include "IDataElement.h"

namespace OSK::PERSISTENCE {

	/// @brief Interfaz para un wrapper de un atributo.
	class OSKAPI_CALL IFieldWrapper : public IDataElement {

	protected:

		inline IFieldWrapper(DataType type) : IDataElement(type) { }

	};


	/// @brief Wrapper para un atributo.
	/// Puede contener una lista de atributos del mismo tipo,
	/// se debe comprobar la cantidad de elementos.
	/// 
	/// @tparam TDataType Tipo de dato.
	template <typename TDataType>
	class FieldWrapper : public IFieldWrapper {

	public:

		/// @brief FieldWrapper vacío.
		FieldWrapper();
		/// @brief FieldWrapper con un único elemento.
		FieldWrapper(const TDataType& data);


		/// @brief Añade un valor a la lista.
		/// @param data Valor.
		inline void AddValue(const TDataType& data) {
			this->data.Insert(data);
		}
		
		/// @brief Actualiza el valor en el índice dado.
		/// @param index Índice dentro de la lista. Debe apuntar a un
		/// índice en el que hay un elemento guardado.
		/// @param data Valor.
		/// 
		/// @pre El índice debe apuntar a un índice en el que 
		/// hay un elemento guardado.
		inline void SetValue(TIndex index, const TDataType& data) {
			this->data.At(index) = data;
		}

		/// @brief Obtiene el valor en el índice dado.
		/// @param index Índice dentro de la lista.
		/// @return Valor.
		/// 
		/// @pre El índice debe ser < a GetFieldCount().
		inline const TDataType& Get(TIndex index) const { return data.At(index); }

		/// @brief Obtiene el valor en el índice dado.
		/// @param index Índice dentro de la lista.
		/// @return Valor.
		/// 
		/// @pre El índice debe ser < a GetFieldCount().
		inline TDataType& GetRef(TIndex index) { return data.At(index); }

		/// @brief Devuelve el número de valores en la lista.
		/// @return Número de valores almacenados.
		inline TSize GetFieldCount() const {
			return data.GetSize();
		}

	private:

		DynamicArray<TDataType> data;

	};

	template class FieldWrapper<std::string>;
	template class FieldWrapper<int>;
	template class FieldWrapper<float>;

}
