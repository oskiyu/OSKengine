#pragma once

#include "Wrapper.h"

#include <OSKengine/Vector3.hpp>

namespace OSKengine {

	template <typename T> public ref class Vector3 : Wrapper<OSK::Vector3_t<T>> {

	public:

		Vector3() : Wrapper(new OSK::Vector3_t<T>()) {

		}
		Vector3(T x, T y, T z) : Wrapper(new OSK::Vector3_t<T>(x, y, z)) {

		}
		Vector3(T value) : Wrapper(new OSK::Vector3_t<T>(value)) {

		}

		inline Vector3 operator+(Vector3^ vec) {
			return Vector3(X + vec.X, Y + vec.Y, Z + vec.Z);
		}
		inline Vector3% operator+=(Vector3^ vec) {
			X += vec.X;
			Y += vec.Y;
			Z += vec.Z;

			return *this;
		}

		inline Vector3 operator-() {
			return Vector3(-X, -Y, -Z);
		}
		inline Vector3 operator-(Vector3^ vec) {
			return Vector3_t(X - vec.X, Y - vec.Y, Z - vec.Z);
		}
		inline Vector3% operator-=(Vector3^ vec) {
			X -= vec.X;
			Y -= vec.Y;
			Z -= vec.Z;

			return *this;
		}

		inline Vector3 operator*(Vector3^ vec) {
			return Vector3(X * vec.X, Y * vec.Y, Z * vec.Z);
		}
		inline Vector3% operator*=(Vector3^ vec) {
			X *= vec.X;
			Y *= vec.Y;
			Z *= vec.Z;

			return *this;
		}
		inline Vector3 operator*(T value) {
			return Vector3(X * value, Y * value, Z * value);
		}
		inline Vector3% operator*=(T value) {
			X *= value;
			Y *= value;
			Z *= value;

			return *this;
		}

		inline Vector3 operator/(Vector3^ vec) {
			return Vector3(X / vec.X, Y / vec.Y, Z / vec.Z);
		}
		Vector3% operator/=(Vector3^ vec) {
			X /= vec;
			Y /= vec;
			Z /= vec;

			return *this;
		}
		Vector3 operator/(T value) {
			return Vector3(X / value, Y / value, Z / value);
		}
		Vector3% operator/=(T value) {
			X /= value;
			Y /= value;
			Z /= value;

			return *this;
		}

		bool operator==(Vector3^ vec) {
			return X == vec.X && Y == vec.Y && Z == vec.Z;
		}
		bool operator!=(Vector3^ vec) {
			return !operator==(vec);
		}

		T GetLenght() {
			return glm::sqrt(X * X + Y * Y + Z * Z);
		}
		T GetDistanceTo(Vector3^ vec) {
			T dx = X - vec.X;
			T dy = Y - vec.Y;
			T dz = Z - vec.Z;

			return glm::sqrt(dx * dx + dy * dy + dz * dz);
		}
		T GetDistanceTo2(Vector3^ vec) {
			T dx = X - vec.X;
			T dy = Y - vec.Y;
			T dz = Z - vec.Z;

			return dx * dx + dy * dy + dz * dz;
		}

		T Dot(Vector3^ vec) {
			return X * vec.X + Y * vec.Y + Z * vec.Z;
		}
		Vector3 Cross(Vector3^ vec) {
			return Vector3(Y * vec.Z - Z * vec.Y, Z * vec.X - X * vec.Z, X * vec.Y - Y * vec.X);
		}

		Vector3 GetNormalized() {
			T length = GetLenght();

			return Vector3(X / length, Y / length, Z / length);
		}
		void Normalize() {
			T length = GetLenght();

			X /= length;
			Y /= length;
			Z /= length;
		}

		template <typename P> inline Vector3<P> ToVec3() {
			return Vector3<P>(X, Y, Z);
		}
		inline Vector3<float_t> ToVector3f() {
			return ToVec3<float>();
		}
		inline Vector3<double_t> ToVector3d() {
			return ToVec3<double_t>();
		}
		inline Vector3<int32_t> ToVector3i() {
			return ToVec3<int32_t>();
		}
		inline Vector3<uint32_t> ToVector3ui() {
			return ToVec3<uint32_t>();
		}

		property T^ X {
		public:
			T^ get() {
				return instance->X;
			}
			void set(T^ value) {
				instance->X = value;
			}
		}
		property T^ Y {
		public:
			T^ get() {
				return instance->Y;
			}
			void set(T^ value) {
				instance->Y = value;
			}
		}
		property T^ Z {
		public:
			T^ get() {
				return instance->Z;
			}
			void set(T^ value) {
				instance->Z = value;
			}
		}

	};

}