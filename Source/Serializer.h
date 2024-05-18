// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#pragma once

#include <json.hpp>

#include "BinaryBlock.h"


namespace OSK::ECS {
	class SavedGameObjectTranslator;
}

namespace OSK::PERSISTENCE {

	// --- JSON: General --- //

	/// @brief Obtiene un nodo JSON que describe un
	/// dato en particular.
	/// @tparam T Tipo de dato.
	/// @param data Dato.
	/// @return Dato serializado.
	/// 
	/// @pre Debe estar definido para el tipo @p T.
	template <typename T>
	nlohmann::json SerializeData(const T& data) = delete;

	/// @brief Construye un dato a partir del JSON
	/// que lo describe.
	/// @tparam T Tipo de dato.
	/// @param data Dato.
	/// @return Dato reconstruido.
	/// 
	/// @pre Debe estar definido para el tipo @p T.
	/// @pre Debe haber sido serializado mediante 
	/// `template <typename T> nlohmann::json SerializeJson(const T& data)`.
	template <typename T>
	T DeserializeData(const nlohmann::json& data) = delete;


	// --- Binary: general --- //
	/// @brief Obtiene un bloque binario que describe un
	/// dato en particular.
	/// @tparam T Tipo de dato.
	/// @param data Dato.
	/// @return Dato serializado.
	/// 
	/// @pre Debe estar definido para el tipo @p T.
	template <typename T>
	BinaryBlock BinarySerializeData(const T& data) = delete;

	/// @brief Construye un dato a partir del bloque binario
	/// que lo describe.
	/// @tparam T Tipo de dato.
	/// @param data Dato.
	/// @return Dato reconstruido.
	/// 
	/// @pre Debe estar definido para el tipo @p T.
	/// @pre Debe haber sido serializado mediante 
	/// `BinarySerializeData(const T& data)`.
	template <typename T>
	T BinaryDeserializeData(BinaryBlockReader* reader) = delete;


	// --- JSON: Scene --- //

	/// @brief Obtiene un nodo JSON que describe un
	/// dato en particular.
	/// @tparam T Tipo de dato.
	/// @param data Dato.
	/// @return Dato serializado.
	/// 
	/// @pre Debe estar definido para el tipo @p T.
	template <typename T>
	nlohmann::json SerializeComponent(const T& data) = delete;

	/// @brief Construye un dato a partir del JSON
	/// que lo describe.
	/// @tparam T Tipo de dato.
	/// @param data Dato.
	/// @param gameObjectTranslator Contiene la tabla para obtener los nuevos índices de los objetos.
	/// @return Dato reconstruido.
	/// 
	/// @pre Debe estar definido para el tipo @p T.
	/// @pre Debe haber sido serializado mediante 
	/// `template <typename T> nlohmann::json SerializeJson(const T& data)`.
	template <typename T>
	T DeserializeComponent(
		const nlohmann::json& data,
		const ECS::SavedGameObjectTranslator& gameObjectTranslator) = delete;

	// --- Binary: Scene --- //

	template <typename T>
	BinaryBlock BinarySerializeComponent(const T& data) = delete;

	template <typename T>
	T BinaryDeserializeComponent(
		BinaryBlockReader* reader,
		const ECS::SavedGameObjectTranslator& gameObjectTranslator) = delete;

}

#define OSK_SERIALIZABLE() \
template <typename T> \
friend ::nlohmann::json OSK::PERSISTENCE::SerializeData<T>(const T& data); \
template <typename T> \
friend T OSK::PERSISTENCE::DeserializeData<T>(const ::nlohmann::json& data); \
template <typename T> \
friend OSK::PERSISTENCE::BinaryBlock OSK::PERSISTENCE::BinarySerializeData<T>(const T& data); \
template <typename T> \
friend T OSK::PERSISTENCE::BinaryDeserializeData<T>(OSK::PERSISTENCE::BinaryBlockReader* reader); \

#define OSK_SERIALIZABLE_COMPONENT() \
template <typename T> \
friend ::nlohmann::json OSK::PERSISTENCE::SerializeComponent<T>(const T& data); \
template <typename T> \
friend T OSK::PERSISTENCE::DeserializeComponent<T>(const ::nlohmann::json& data, const ::OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator); \
template <typename T> \
friend OSK::PERSISTENCE::BinaryBlock OSK::PERSISTENCE::BinarySerializeComponent<T>(const T& data); \
template <typename T> \
friend T OSK::PERSISTENCE::BinaryDeserializeComponent<T>(OSK::PERSISTENCE::BinaryBlockReader* reader, const ::OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator); \


#define OSK_SERIALIZATION(type) \
namespace OSK::PERSISTENCE { \
template <> \
::nlohmann::json SerializeData<type>(const type& data); \
template <> \
type DeserializeData<type>(const ::nlohmann::json& json); \
template <> \
OSK::PERSISTENCE::BinaryBlock BinarySerializeData<type>(const type& data); \
template <> \
type BinaryDeserializeData<type>(OSK::PERSISTENCE::BinaryBlockReader* reader); \
}

#define OSK_COMPONENT_SERIALIZATION(type) \
namespace OSK::PERSISTENCE { \
template <> \
::nlohmann::json SerializeComponent<type>(const type& data); \
template <> \
type DeserializeComponent<type>(const ::nlohmann::json& json, const ::OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator); \
template <> \
OSK::PERSISTENCE::BinaryBlock BinarySerializeComponent<type>(const type& data); \
template <> \
type BinaryDeserializeComponent<type>(OSK::PERSISTENCE::BinaryBlockReader* reader, const ::OSK::ECS::SavedGameObjectTranslator& gameObjectTranslator); \
}
