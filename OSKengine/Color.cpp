#include "Color.h"

namespace OSK {

	Color::Color() {
		Red = 0.0f;
		Green = 0.0f;
		Blue = 0.0f;
		Alpha = 1.0f;
	}

	Color::Color(const float_t& r, const float_t& g, const float_t& b) {
		Red = r;
		Green = g;
		Blue = b;
		Alpha = 1.0f;
	}

	Color::Color(const float_t& r, const float_t& g, const float_t& b, const float_t& a) {
		Red = r;
		Green = g;
		Blue = b;
		Alpha = a;
	}

	Color::Color(const float_t& value) {
		Red = value;
		Green = value;
		Blue = value;
		Alpha = 1.0f;
	}

	Color Color::operator*(const float_t& value) const {
		return Color(Red, Green, Blue, value);
	}

	void Color::Update(float_t r, float_t g, float_t b) {
		Red = r;
		Green = g;
		Blue = b;
	}

}