#pragma once

#include <OSKengine/OSKmacros.h>

#include <OSKengine/HashMap.hpp>

/// @brief Registro con todos los modelos de coches disponibles
/// y sus assets.
class CarRegistry {

public:

	CarRegistry() = delete;
	OSK_DISABLE_COPY(CarRegistry);
	OSK_DISABLE_MOVE(CarRegistry);

	static void LoadRegistry();
	static std::string_view GetAssetsRoute(std::string_view carName);

private:

	static std::unordered_map<std::string, std::string, OSK::StringHasher, std::equal_to<>> m_registry;

};
