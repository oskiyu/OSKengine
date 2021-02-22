#include "ToString.h"

namespace OSK {

	std::string ToString(const bool& value) {
		if (value)
			return "TRUE";
		return "FALSE";
	}

	std::string ToString(const OSK::Vector2& vector2) {
		return "{ " + std::to_string(vector2.X) + "; " + std::to_string(vector2.Y) + " }";
	}

	std::string ToString(const OSK::Vector3& vector3) {
		return "{ " + std::to_string(vector3.X) + "; " + std::to_string(vector3.Y) + "; " + std::to_string(vector3.Z) + " }";
	}
	
	std::string ToString(const OSK::Vector4& vector4) {
		return "{ " + std::to_string(vector4.X) + "; " + std::to_string(vector4.Y) + "; " + std::to_string(vector4.Z) + "; " + std::to_string(vector4.W) + "}";
	}

	std::string ToString(const OSK::Transform& transform) {
		//return "Position: " + ToString(transform.GlobalPosition) + " (" + ToString(transform.PositionOffset) + " + " + ToString(transform.Position) + ")\n" + "Rotation: " + ToString(transform.GlobalRotation) + " (" + ToString(transform.RotationOffset) + " + " + ToString(transform.Rotation) + ")\n" + "Scale: " + ToString(transform.GlobalScale);
		return "";
	}

}