#include "Lights.h"

#include "Math.h"

using namespace OSK;
using namespace OSK::GRAPHICS;
using namespace OSK::PERSISTENCE;


template <>
nlohmann::json OSK::PERSISTENCE::SerializeData<OSK::GRAPHICS::DirectionalLight>(const OSK::GRAPHICS::DirectionalLight& data) {
	nlohmann::json output{};

	output["color"] = SerializeData<Color>(data.color);

	output["direction"] = SerializeVector3<Vector3f>({ data.directionAndIntensity.x, data.directionAndIntensity.y, data.directionAndIntensity.z });

	output["intensity"] = data.directionAndIntensity.w;

	return output;
}

template <>
OSK::GRAPHICS::DirectionalLight PERSISTENCE::DeserializeData<OSK::GRAPHICS::DirectionalLight>(const nlohmann::json& json) {
	DirectionalLight output{};

	output.color = DeserializeData<Color>(json["color"]);
	const Vector3f direction = DeserializeVector3<Vector3f>(json["direction"]);

	output.directionAndIntensity = Vector4f{
		direction.x,
		direction.y,
		direction.z,
		json["intensity"]
	};

	return output;
}


template <>
BinaryBlock OSK::PERSISTENCE::BinarySerializeData<OSK::GRAPHICS::DirectionalLight>(const OSK::GRAPHICS::DirectionalLight& data) {
	BinaryBlock output{};

	output.AppendBlock(BinarySerializeData<Color>(data.color));
	output.AppendBlock(SerializeBinaryVector3<Vector3f>({ data.directionAndIntensity.x, data.directionAndIntensity.y, data.directionAndIntensity.z }));
	output.Write<float>(data.directionAndIntensity.w);

	return output;
}

template <>
OSK::GRAPHICS::DirectionalLight PERSISTENCE::BinaryDeserializeData<OSK::GRAPHICS::DirectionalLight>(BinaryBlockReader* reader) {
	DirectionalLight output{};

	output.color = BinaryDeserializeData<Color>(reader);
	const Vector3f direction = DeserializeBinaryVector3<Vector3f, float>(reader);

	output.directionAndIntensity = Vector4f{
		direction.x,
		direction.y,
		direction.z,
		reader->Read<float>()
	};

	return output;
}
