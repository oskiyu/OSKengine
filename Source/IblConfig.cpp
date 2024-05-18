#include "PbrIblConfig.h"

using namespace OSK::PERSISTENCE;


template <>
nlohmann::json OSK::PERSISTENCE::SerializeData<OSK::GRAPHICS::PbrIblConfig>(const OSK::GRAPHICS::PbrIblConfig& data) {
	nlohmann::json output{};

	output["irradianceStrength"] = data.irradianceStrength;
	output["specularStrength"] = data.specularStrength;
	output["radianceStrength"] = data.radianceStrength;
	output["emissiveStrength"] = data.emissiveStrength;

	return output;
}

template <>
OSK::GRAPHICS::PbrIblConfig OSK::PERSISTENCE::DeserializeData<OSK::GRAPHICS::PbrIblConfig>(const nlohmann::json& json) {
	OSK::GRAPHICS::PbrIblConfig output{};

	output.irradianceStrength = json["irradianceStrength"];
	output.specularStrength = json["specularStrength"];
	output.radianceStrength = json["radianceStrength"];
	output.emissiveStrength = json["emissiveStrength"];

	return output;
}


template <>
BinaryBlock OSK::PERSISTENCE::BinarySerializeData<OSK::GRAPHICS::PbrIblConfig>(const OSK::GRAPHICS::PbrIblConfig& data) {
	BinaryBlock output{};

	output.Write<float>(data.irradianceStrength);
	output.Write<float>(data.specularStrength);
	output.Write<float>(data.radianceStrength);
	output.Write<float>(data.emissiveStrength);

	return output;
}

template <>
OSK::GRAPHICS::PbrIblConfig OSK::PERSISTENCE::BinaryDeserializeData<OSK::GRAPHICS::PbrIblConfig>(BinaryBlockReader* reader) {
	OSK::GRAPHICS::PbrIblConfig output{};

	output.irradianceStrength = reader->Read<float>();
	output.specularStrength = reader->Read<float>();
	output.radianceStrength = reader->Read<float>();
	output.emissiveStrength = reader->Read<float>();

	return output;
}
