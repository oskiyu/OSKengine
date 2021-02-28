#include "Color.h"

namespace OSK {

	Color::Color() {
		Red = 0.0f;
		Green = 0.0f;
		Blue = 0.0f;
		Alpha = 1.0f;
	}

	Color::Color(float r, float g, float b) {
		Red = r;
		Green = g;
		Blue = b;
		Alpha = 1.0f;
	}

	Color::Color(float r, float g, float b, float a) {
		Red = r;
		Green = g;
		Blue = b;
		Alpha = a;
	}

	Color::Color(float value) {
		Red = value;
		Green = value;
		Blue = value;
		Alpha = 1.0f;
	}

	Color Color::operator*(float value) const {
		return Color(Red, Green, Blue, value);
	}

	glm::vec4 Color::ToGLM() const {
		return glm::vec4(Red, Green, Blue, Alpha);
	}

	void Color::Update(float r, float g, float b) {
		Red = r;
		Green = g;
		Blue = b;
	}

}