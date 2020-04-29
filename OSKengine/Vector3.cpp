#include "Vector3.h"

namespace OSK {

	Vector3::Vector3() {
		X = 0;
		Y = 0;
		Z = 0;
	}

	Vector3::Vector3(const float_t& x, const float_t& y, const float_t& z) {
		X = x;
		Y = y;
		Z = z;
	}

	Vector3::Vector3(const float_t& value) {
		X = value;
		Y = value;
		Z = value;
	}

	Vector3::Vector3(const glm::vec3& vec) {
		X = vec.x;
		Y = vec.y;
		Z = vec.z;
	}

	Vector3 Vector3::operator+(const Vector3& vec) const {
		return Vector3(X + vec.X, Y + vec.Y, Z + vec.Z);
	}

	Vector3 Vector3::operator-() const {
		return Vector3(-X, -Y, -Z);
	}

	Vector3 Vector3::operator-(const Vector3& vec) const{
		return Vector3(X - vec.X, Y - vec.Y, Z - vec.Z);
	}

	Vector3 Vector3::operator*(const Vector3& vec) const {
		return Vector3(X * vec.X, Y * vec.Y, Z * vec.Z);
	}

	Vector3 Vector3::operator*(const float_t& value) const {
		return Vector3(X * value, Y * value, Z * value);
	}

	Vector3 Vector3::operator/(const Vector3& vec) const {
		return Vector3(X / vec.X, Y / vec.Y, Z / vec.Z);
	}

	Vector3 Vector3::operator/(const float_t& value) const {
		return Vector3(X / value, Y / value, Z / value);
	}

	float_t Vector3::GetLenght() const {
		return glm::sqrt(X * X + Y * Y + Z * Z);
	}

	float_t Vector3::GetDistanceTo(const Vector3& vec) const {
		float_t dx = X - vec.X;
		float_t dy = Y - vec.Y;
		float_t dz = Z - vec.Z;

		return glm::sqrt(dx * dx + dy * dy + dz * dz);
	}

	float_t Vector3::Dot(const Vector3& vec) const {
		return X * vec.X + Y * vec.Y + Z * vec.Z;
	}

	Vector3 Vector3::Cross(const Vector3& vec) const {
		return Vector3(Y * vec.Z - Z * vec.Y, Z * vec.X - X * vec.Z, X * vec.Y - Y * vec.X);
	}

	Vector3 Vector3::GetNormalized() const {
		float_t length = GetLenght();

		return Vector3(X / length, Y / length, Z / length);
	}

	void Vector3::Normalize() {
		float_t length = GetLenght();

		X /= length;
		Y /= length;
		Z /= length;
	}

	glm::vec3 Vector3::ToGLM() const {
		return glm::vec3(X, Y, Z);
	}

}