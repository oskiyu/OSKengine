#include "PbrIblConfig.h"


template <>
nlohmann::json OSK::PERSISTENCE::SerializeJson<OSK::GRAPHICS::PbrIblConfig>(const OSK::GRAPHICS::PbrIblConfig& data) {
	nlohmann::json output{};

	output["irradianceStrength"] = data.irradianceStrength;
	output["specularStrength"] = data.specularStrength;
	output["radianceStrength"] = data.radianceStrength;
	output["emissiveStrength"] = data.emissiveStrength;

	return output;
}

template <>
OSK::GRAPHICS::PbrIblConfig OSK::PERSISTENCE::DeserializeJson<OSK::GRAPHICS::PbrIblConfig>(const nlohmann::json& json) {
	OSK::GRAPHICS::PbrIblConfig output{};

	output.irradianceStrength = json["irradianceStrength"];
	output.specularStrength = json["specularStrength"];
	output.radianceStrength = json["radianceStrength"];
	output.emissiveStrength = json["emissiveStrength"];

	return output;
}
