#include "Color.hpp"

namespace OSK {

	// ------- BUILDS --------- //

	Color const Color::Empty	= Color(0.0f, 0.0f, 0.0f, 0.0f);
	Color const Color::Black	= Color(0.0f, 0.0f, 0.0f);
	Color const Color::White	= Color(1.0f, 1.0f, 1.0f);
	Color const Color::Blue		= Color(0.0f, 0.0f, 1.0f);
	Color const Color::Purple	= Color(1.0f, 0.0f, 1.0f);
	Color const Color::Red		= Color(1.0f, 0.0f, 0.0f);
	Color const Color::Yellow	= Color(1.0f, 1.0f, 0.0f);
	Color const Color::Green	= Color(0.0f, 1.0f, 0.0f);

}


template <>
nlohmann::json OSK::PERSISTENCE::SerializeData<OSK::Color>(const OSK::Color& data) {
	nlohmann::json output{};

	output["r"] = data.red;
	output["g"] = data.green;
	output["b"] = data.blue;
	output["a"] = data.alpha;

	return output;
}

template <>
OSK::Color OSK::PERSISTENCE::DeserializeData<OSK::Color>(const nlohmann::json& json) {
	return Color(
		json["r"],
		json["g"],
		json["b"],
		json["a"]
	);
}


template <>
OSK::PERSISTENCE::BinaryBlock OSK::PERSISTENCE::BinarySerializeData<OSK::Color>(const OSK::Color& data) {
	BinaryBlock output{};

	output.Write<float>(data.red);
	output.Write<float>(data.green);
	output.Write<float>(data.blue);
	output.Write<float>(data.alpha);

	return output;
}

template <>
OSK::Color OSK::PERSISTENCE::BinaryDeserializeData<OSK::Color>(BinaryBlockReader* reader) {
	return Color(
		reader->Read<float>(),
		reader->Read<float>(),
		reader->Read<float>(),
		reader->Read<float>()
	);
}

