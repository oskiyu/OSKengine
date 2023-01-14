// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#pragma once

#include "DynamicArray.hpp"

namespace OSK::PERSISTENCE {

	/// @brief Tipo de dato que se puede almacenar en un FieldWrapper.
	enum class FieldType {
		STRING,
		INT,
		FLOAT,
		ECS_ID
	};

	/// @brief Tipo de contenedor que se puede almacenar en un FieldWrapper.
	enum class ContainerType {
		/// @brief Se almacena un único valor.
		SINGLE,
		/// @brief Se almacena una lista de valores.
		LIST
	};


	/// @brief Interfaz para un wrapper de un atributo.
	class IFieldWrapper {

		/// @brief Tipo de atributo.
		/// @return Tipo de atributo. 
		virtual FieldType GetFieldType() const = 0;

		/// @brief Tipo de contenedor.
		/// @return Tipo de contenedor. 
		virtual ContainerType GetContainerType() const = 0;

	};


	/// @brief Wrapper para un atributo.
	/// @tparam TDataType Tipo de dato.
	template <typename TDataType>
	class SingleFieldWrapper : public IFieldWrapper {

	public:

		FieldType GetFieldType() const override;
		ContainerType GetContainerType() const override {
			return ContainerType::SINGLE;
		}

		inline const TDataType& Get() const { return data; }
		inline TDataType& GetRef() { return data; }

	private:

		TDataType data;

	};

	/// @brief Wrapper para un atributo.
	/// @tparam TDataType Tipo de dato.
	template <typename TDataType>
	class ListFieldWrapper : public IFieldWrapper {

	public:

		FieldType GetFieldType() const override;
		ContainerType GetContainerType() const override {
			return ContainerType::LIST;
		}

		inline const DynamicArray<TDataType>& Get() const { return data; }
		inline DynamicArray<TDataType>& GetRef() { return data; }

	private:

		DynamicArray<TDataType> data;

	};

	template class SingleFieldWrapper<std::string>;
	template class SingleFieldWrapper<int>;
	template class SingleFieldWrapper<float>;

	template class ListFieldWrapper<std::string>;
	template class ListFieldWrapper<int>;
	template class ListFieldWrapper<float>;

}
