#include "Color.hpp"

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

	Color Color::operator+(const Color& other) const {
		return Color(Red + other.Red * other.Alpha, Green + other.Green * other.Alpha, Blue + other.Blue * other.Alpha, Alpha + other.Alpha);
	}

	glm::vec4 Color::ToGlm() const {
		return glm::vec4(Red, Green, Blue, Alpha);
	}

	void Color::Update(float r, float g, float b) {
		Red = r;
		Green = g;
		Blue = b;
	}

	// ------- BUILDS --------- //

	Color Color::BLACK() {
		return Color(0.0f, 0.0f, 0.0f);
	}

	Color Color::WHITE() {
		return Color(1.0f);
	}

	Color Color::BLUE() {
		return Color(0.0f, 1.0f, 1.0f);
	}

	Color Color::PURPLE() {
		return Color(1.0f, 0.0f, 1.0f);
	}

	Color Color::RED() {
		return Color(1.0f, 0.0f, 0.0f);
	}

	Color Color::YELLOW() {
		return Color(1.0f, 1.0f, 0.0f);
	}

	Color Color::GREEN() {
		return Color(0.0f, 1.0f, 0.0f);
	}

}
