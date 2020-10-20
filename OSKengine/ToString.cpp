#include "ToString.h"

#include "GraphicsAPIenum.h"

namespace OSK {

	OSK_INFO_GLOBAL 
	std::string ToString(const bool& value) {
		if (value)
			return "TRUE";
		return "FALSE";
	}


	OSK_INFO_GLOBAL 
	std::string ToString(const OSK::Vector2& vector2) {
		return "{ " + std::to_string(vector2.X) + "; " + std::to_string(vector2.Y) + " }";
	}


	OSK_INFO_GLOBAL 
	std::string ToString(const OSK::Vector3& vector3) {
		return "{ " + std::to_string(vector3.X) + "; " + std::to_string(vector3.Y) + "; " + std::to_string(vector3.Z) + " }";
	}
	

	OSK_INFO_GLOBAL 
	std::string ToString(const OSK::Vector4& vector4) {
		return "{ " + std::to_string(vector4.X) + "; " + std::to_string(vector4.Y) + "; " + std::to_string(vector4.Z) + "; " + std::to_string(vector4.W) + "}";
	}


	OSK_INFO_GLOBAL 
	std::string ToString(const OSK::Transform& transform) {
		//return "Position: " + ToString(transform.GlobalPosition) + " (" + ToString(transform.PositionOffset) + " + " + ToString(transform.Position) + ")\n" + "Rotation: " + ToString(transform.GlobalRotation) + " (" + ToString(transform.RotationOffset) + " + " + ToString(transform.Rotation) + ")\n" + "Scale: " + ToString(transform.GlobalScale);
		return "";
	}


	OSK_INFO_GLOBAL
	OSK::Vector2 GetTextSize(const std::string& texto, OldFont& fuente, const Vector2& size) {
		//Char que se tomará como referencia en cuanto al tamaño de las letras.
		OldFontChar reference = fuente.Characters['A'];
		std::string::const_iterator iterador;

		if (texto == "")
			return Vector2(0.0f);

		float_t temporalSizeX = 0.0f;
		float_t sizeX = 0.0f;
		float_t sizeY = reference.Bearing.y * size.Y + reference.Size.y * size.Y;

		for (iterador = texto.begin(); iterador != texto.end(); iterador++) {
			OldFontChar character = fuente.Characters[*iterador];
			if (*iterador == '\n') {
				if (temporalSizeX > sizeX) {
					sizeX = temporalSizeX;
					temporalSizeX = 0;

					sizeY += reference.Bearing.y * size.Y + reference.Size.y * size.Y;
				}
			}
			if (*iterador == '\t') {
				temporalSizeX += (character.Bearing.x * size.X + character.Size.x * size.X) * SPACES_PER_TAB;
			}

			temporalSizeX += character.Bearing.x * size.X + character.Size.x * size.X;
		}

		if (temporalSizeX > sizeX)
			sizeX = temporalSizeX;

		return Vector2(sizeX, sizeY);
	}


	OSK_INFO_GLOBAL
	std::string ToString(const GraphicsAPI& graphicsAPI) {
		if (graphicsAPI == GraphicsAPI::OPENGL)
			return "OpenGL";

		return "Vulkan";
	}


	OSK_INFO_GLOBAL
	Vector2 GetTextSize(const std::string& texto, OldFont& fuente, const float_t& size) {
		return GetTextSize(texto, fuente, Vector2(size));
	}

}