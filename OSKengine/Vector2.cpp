#include "Vector2.h"

namespace OSK {

	/*Vector2::Vector2() {
		X = 0;
		Y = 0;
	}

	Vector2::Vector2(const float_t& x, const float_t& y) {
		X = x;
		Y = y;
	}

	Vector2::Vector2(const float_t& value) {
		X = value;
		Y = value;
	}

	Vector2::Vector2(const glm::vec2& vec) {
		X = vec.x;
		Y = vec.y;
	}

	Vector2 Vector2::operator+(const Vector2& vec) const {
		return Vector2(X + vec.X, Y + vec.Y);
	}

	Vector2 Vector2::operator-() const {
		return Vector2(-X, -Y);
	}

	Vector2 Vector2::operator-(const Vector2& vec) const {
		return Vector2(X - vec.X, Y - vec.Y);
	}
	
	Vector2 Vector2::operator*(const Vector2& vec) const {
		return Vector2(X * vec.X, Y * vec.Y);
	}

	Vector2 Vector2::operator*(const float_t& value) const {
		return Vector2(X * value, Y * value);
	}

	Vector2 Vector2::operator/(const float_t& value) const {
		return Vector2(X / value, Y / value);
	}

	Vector2 Vector2::operator/(const Vector2& vec) const {
		return Vector2(X / vec.X, Y / vec.Y);
	}

	float_t Vector2::GetLength() const {
		return glm::sqrt(X * X + Y * Y);
	}

	float_t Vector2::GetDistanceTo(const OSK::Vector2& vec) const {
		float_t dx = X - vec.X;
		float_t dy = Y - vec.Y;
		
		return glm::sqrt(dx * dx + dy * dy);
	}

	float_t Vector2::Dot(const Vector2& vec) const {
		return X * vec.X + Y * vec.Y;
	}

	Vector2 Vector2::GetNormalized() const {
		float_t length = GetLength();

		return Vector2(X / length, Y / length);
	}

	void Vector2::Normalize() {
		float_t length = GetLength();

		X /= length;
		Y /= length;
	}

	glm::vec2 Vector2::ToGLM() const {
		return glm::vec2(X, Y);
	}*/

}