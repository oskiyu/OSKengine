// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#pragma once

#include <json.hpp>

namespace OSK::PERSISTENCE {

	// --- JSON: Scene Editor --- //


	/// @brief Obtiene un nodo JSON que describe un
	/// dato en particular.
	/// @tparam T Tipo de dato.
	/// @param data Dato.
	/// @return Dato serializado.
	/// 
	/// @pre Debe estar definido para el tipo @p T.
	template <typename T>
	nlohmann::json SerializeJson(const T& data) = delete;

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
	T DeserializeJson(const nlohmann::json& data) = delete;

	template <typename TSystem, typename TConfigType>
	void ConfigureFromSerialization(const nlohmann::json& data, TSystem* system, const TConfigType& config);

}

#define OSK_SERIALIZABLE() \
template <typename T> \
friend ::nlohmann::json OSK::PERSISTENCE::SerializeJson<T>(const T& data); \
template <typename T> \
friend T OSK::PERSISTENCE::DeserializeJson<T>(const ::nlohmann::json& data); \

#define OSK_SERIALIZATION(type) \
namespace OSK::PERSISTENCE { \
template <> \
nlohmann::json SerializeJson<type>(const type& data); \
template <> \
type DeserializeJson<type>(const nlohmann::json& json); \
} 

#define OSK_SYSTEM_SERIALIZATION(system, configSystem) \
namespace OSK::PERSISTENCE { \
template <> \
nlohmann::json SerializeJson<system>(const system& data); \
template <> \
void ConfigureFromSerialization<system, configSystem>(const nlohmann::json& json, system* system, const configSystem& config); \
} 

