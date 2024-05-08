#include "Lights.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


template <>
nlohmann::json OSK::PERSISTENCE::SerializeJson<OSK::GRAPHICS::DirectionalLight>(const OSK::GRAPHICS::DirectionalLight& data) {
	nlohmann::json output{};

	output["color"] = SerializeJson<Color>(data.color);

	output["direction"]["x"] = data.directionAndIntensity.x;
	output["direction"]["y"] = data.directionAndIntensity.x;
	output["direction"]["z"] = data.directionAndIntensity.x;

	output["intensity"] = data.directionAndIntensity.W;

	return output;
}

template <>
OSK::GRAPHICS::DirectionalLight PERSISTENCE::DeserializeJson<OSK::GRAPHICS::DirectionalLight>(const nlohmann::json& json) {
	DirectionalLight output{};

	output.color = DeserializeJson<Color>(json["color"]);

	output.directionAndIntensity = Vector4f{
		json["direction"]["x"],
		json["direction"]["x"],
		json["direction"]["x"],
		json["intensity"]
	};

	return output;
}
