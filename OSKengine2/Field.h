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

		/// @brief FieldWrapper vac�o.
		FieldWrapper();
		/// @brief FieldWrapper con un �nico elemento.
		FieldWrapper(const TDataType& data);


		/// @brief A�ade un valor a la lista.
		/// @param data Valor.
		inline void AddValue(const TDataType& data) {
			this->data.Insert(data);
		}
		
		/// @brief Actualiza el valor en el �ndice dado.
		/// @param index �ndice dentro de la lista. Debe apuntar a un
		/// �ndice en el que hay un elemento guardado.
		/// @param data Valor.
		/// 
		/// @pre El �ndice debe apuntar a un �ndice en el que 
		/// hay un elemento guardado.
		inline void SetValue(UIndex64 index, const TDataType& data) {
			this->data.At(index) = data;
		}

		/// @brief Obtiene el valor en el �ndice dado.
		/// @param index �ndice dentro de la lista.
		/// @return Valor.
		/// 
		/// @pre El �ndice debe ser < a GetFieldCount().
		inline const TDataType& Get(UIndex64 index) const { return data.At(index); }

		/// @brief Obtiene el valor en el �ndice dado.
		/// @param index �ndice dentro de la lista.
		/// @return Valor.
		/// 
		/// @pre El �ndice debe ser < a GetFieldCount().
		inline TDataType& GetRef(UIndex64 index) { return data.At(index); }

		/// @brief Devuelve el n�mero de valores en la lista.
		/// @return N�mero de valores almacenados.
		inline USize64 GetFieldCount() const {
			return data.GetSize();
		}

	private:

		DynamicArray<TDataType> data;

	};

	template class FieldWrapper<std::string>;
	template class FieldWrapper<int>;
	template class FieldWrapper<float>;

}
