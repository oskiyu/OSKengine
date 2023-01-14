// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#pragma once

#include "DynamicArray.hpp"
#include "HashMap.hpp"
#include <string>

namespace OSK {

	enum class FieldType {
		STRING,
		FLOAT,
		ID
	};


	enum class ContainerType {
		SINGLE,
		LIST
	};

	/// @brief Interfaz para un wrapper de un atributo.
	class IFieldWrapper {

		/// @brief Tipo de atributo.
		/// @return Tipo de atributo. 
		virtual FieldType GetFieldType() const = 0;

	};

	/// @brief Wrapper para un atributo.
	/// @tparam TDataType Tipo de dato.
	template <typename TDataType>
	class SingleFieldWrapper : public IFieldWrapper {

	public:

		inline TDataType& GetRef() { return data; }

		inline const TDataType& Get() const { return data; }

	private:

		TDataType data;

	};

	/// @brief Wrapper para un atributo.
	/// @tparam TDataType Tipo de dato.
	template <typename TDataType>
	class ListFieldWrapper : public IFieldWrapper {

	public:

		inline DynamicArray<TDataType>& GetRef() { return data; }

		inline const DynamicArray<TDataType>& Get() const { return data; }

	private:

		DynamicArray<TDataType> data;

	};

	using StringFieldWrapper = SingleFieldWrapper<std::string>;
	using IntFieldWrapper = SingleFieldWrapper<int>;
	using FloatFieldWrapper = SingleFieldWrapper<float>;

	using StringListFieldWrapper = ListFieldWrapper<std::string>;
	using IntListFieldWrapper = ListFieldWrapper<int>;
	using FloatListFieldWrapper = ListFieldWrapper<float>;

	class Data {
		 
	public:

		void SetField(const std::string& key, const IFieldWrapper& field) {
			fields.Insert(key, field);
		}

		const IFieldWrapper& GetField(const std::string& key) const {
			return fields.Get(key);
		}

	private:

		HashMap<std::string, IFieldWrapper> fields;

	};

	class ISerializable {

	public:

		virtual Data Serialize() const = 0;

	};

	class ISerializer {

	public:

		virtual void Serialize(const Data& data, const std::string& path) = 0;

	};

	class IDeserializer {

	public:

		virtual Data Deserialize(const std::string& path) const = 0;

	};

}
