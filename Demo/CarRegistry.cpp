#include "CarRegistry.h"

#include <OSKengine/FileIO.h>
#include <OSKengine/Assert.h>
#include <OSKengine/InvalidArgumentException.h>

#include <json.hpp>

std::unordered_map<std::string, std::string, OSK::StringHasher, std::equal_to<>> CarRegistry::m_registry;


void CarRegistry::LoadRegistry() {
	const auto file = nlohmann::json::parse(OSK::IO::FileIO::ReadFromFile("Resources/Assets/Cars/registry.json"));

	for (const auto& [key, value] : file.items()) {
		m_registry[key] = static_cast<std::string>(value);
	}
}

std::string_view CarRegistry::GetAssetsRoute(std::string_view carName) {
	OSK_ASSERT(m_registry.contains(carName), OSK::InvalidArgumentException(std::format("No existe el coche {}.", carName)));

	return m_registry.find(carName)->second;
}
