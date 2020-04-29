#include "Vector4.h"

namespace OSK {

	Vector4::Vector4() {
		X = 0;
		Y = 0;
		Z = 0;
		W = 0;
	}

	Vector4::Vector4(const float_t& x, const float_t& y, const float_t& z, const float_t& w) {
		X = x;
		Y = y;
		Z = z;
		W = w;
	}

	Vector4::Vector4(const float_t& value) {
		X = value;
		Y = value;
		Z = value;
		W = value;
	}

	Vector4::Vector4(const glm::vec4& vec) {
		X = vec.x;
		Y = vec.y;
		Z = vec.z;
		W = vec.w;
	}

	Vector4 Vector4::operator+(const Vector4& vec) const {
		return Vector4(X + vec.X, Y + vec.Y, Z + vec.Z, W + vec.W);
	}

	Vector4 Vector4::operator-() const {
		return Vector4(-X, -Y, -Z, -W);
	}

	Vector4 Vector4::operator-(const Vector4& vec) const {
		return Vector4(X - vec.X, Y - vec.Y, Z - vec.Z, W - vec.W);
	}

	Vector4 Vector4::operator*(const Vector4& vec) const {
		return Vector4(X * vec.X, Y * vec.Y, Z * vec.Z, W * vec.W);
	}

	Vector4 Vector4::operator*(const float_t& value) const {
		return Vector4(X * value, Y * value, Z * value, W * value);
	}

	Vector4 Vector4::operator/(const Vector4& vec) const {
		return Vector4(X / vec.X, Y / vec.Y, Z / vec.Z, W / vec.W);
	}

	Vector4 Vector4::operator/(const float_t& value) const {
		return Vector4(X / value, Y / value, Z / value, W / value);
	}

	Vector2 Vector4::GetRectanglePosition() const {
		return Vector2(X, Y);
	}

	Vector2 Vector4::GetRectangleSize() const {
		return Vector2(Z, W);
	}

	float_t Vector4::GetRectangleWidth() const {
		return Z;
	}

	float_t Vector4::GetRectangleHeight() const {
		return W;
	}

	float_t Vector4::GetRectangleTop() const {
		return Y + W;
	}

	float_t Vector4::GetRectangleRight() const {
		return X + Z;
	}

	Vector2 Vector4::GetRectangleMiddlePoint() const {
		return Vector2(X + GetRectangleWidth() / 2, Y + GetRectangleHeight() / 2);
	}

	bool Vector4::Intersects(const Vector4& vec) const {
		return X < vec.X + vec.Z && X + Z > vec.X && Y < vec.Y + vec.W && W + Y > vec.Y;
	}

	glm::vec4 Vector4::ToGLM() const {
		return glm::vec4(X, Y, Z, W);
	}

}