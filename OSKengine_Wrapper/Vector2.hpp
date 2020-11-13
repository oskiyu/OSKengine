#pragma once

#include "Wrapper.h"

#include <OSKengine/Vector2.hpp>

template <typename T> public ref class Vector2_t : Wrapper<OSK::Vector2_t<T>> {

	Vector2_t() : Wrapper(new OSK::Vector2_t<T>()) {

	}
	Vector2_t(T x, T y) : Wrapper(new OSK::Vector2_t<T>(x, y)) {

	}
	Vector2_t(T value) : Wrapper(new OSK::Vector2_t<T>(value)) {

	}

	inline Vector2_t operator+(Vector2_t^ vec) {
		return Vector2_t(X + vec.X, Y + vec.Y);
	}
	inline Vector2_t% operator+=(Vector2_t^ vec) {
		X += vec.X;
		Y += vec.Y;

		return *this;
	}

	inline Vector2_t operator-() {
		return Vector2_t(-X, -Y);
	}
	inline Vector2_t operator-(Vector2_t^ vec) {
		return Vector2_t(X - vec.X, Y - vec.Y);
	}
	inline Vector2_t% operator-=(Vector2_t^ vec) {
		X -= vec.X;
		Y -= vec.Y;

		return *this;
	}

	inline Vector2_t operator*(Vector2_t^ vec) {
		return Vector2_t(X * vec.X, Y * vec.Y);
	}
	inline Vector2_t% operator*=(Vector2_t^ vec) {
		X *= vec.X;
		Y *= vec.Y;

		return *this;
	}
	inline Vector2_t operator*(T value) {
		return Vector2_t(X * value, Y * value);
	}
	inline Vector2_t% operator*=(T value) {
		X *= value;
		Y *= value;

		return *this;
	}

	inline Vector2_t operator/(T value) {
		return Vector2_t(X / value, Y / value);
	}
	inline Vector2_t% operator/=(Vector2_t^ vec) {
		X /= vec.X;
		Y /= vec.Y;

		return *this;
	}
	inline Vector2_t operator/(Vector2_t^ vec) {
		return Vector2_t(X / vec.X, Y / vec.Y);
	}
	inline Vector2_t% operator/=(T value) {
		X /= value;
		Y /= value;

		return *this;
	}

	bool operator==(Vector2_t^ vec) {
		return X == vec.X && Y == vec.Y;
	}
	bool operator!=(Vector2_t^ vec) {
		return !operator==(vec);
	}

	inline T GetLength() {
		return instance->GetLength();
	}
	inline T GetDistanceTo(Vector2_t^ vec) {
		return instance->GetDistanceTo(vec->instance);
	}

	inline T Dot(Vector2_t^ vec) {
		return instance->Dot(vec->instance);
	}

	Vector2_t GetNormalized() {
		T length = GetLength();

		return Vector2_t(X / length, Y / length);
	}
	void Normalize() {
		instance->Normalize();
	}

	template <typename P> inline Vector2_t<P> ToVec2() {
		return Vector2_t<P>(X, Y);
	}

	inline Vector2_t<float> ToVector2f() {
		return ToVec2<float>();
	}
	inline Vector2_t<double> ToVector2d() {
		return ToVec2<double_t>();
	}
	inline Vector2_t<int> ToVector2i() {
		return ToVec2<int32_t>();
	}
	inline Vector2_t<unsigned int> ToVector2ui() {
		return ToVec2<uint32_t>();
	}

	property T X {
	public:
		T get() {
			return instance->X;
		}
		void set(T value) {
			instance->X = value;
		}
	}
	property T Y {
	public:
		T get() {
			return instance->Y;
		}
		void set(T value) {
			instance->Y = value;
		}
	}

};