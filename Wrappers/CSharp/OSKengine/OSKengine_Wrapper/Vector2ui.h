#pragma once

#include "Wrapper.h"

#include <OSKengine/Vector2.hpp>

namespace OSKengine {

#define Vector2 Vector2ui
#define T unsigned int

	public ref class Vector2 : Wrapper<OSK::Vector2_t<T>> {

	public:

		Vector2() : Wrapper(new OSK::Vector2_t<T>()) {

		}
		Vector2(T x, T y) : Wrapper(new OSK::Vector2_t<T>(x, y)) {

		}
		Vector2(T value) : Wrapper(new OSK::Vector2_t<T>(value)) {

		}

		inline static Vector2^ operator+(Vector2^ dis, Vector2^ vec) {
			return gcnew Vector2(dis->X + vec->X, dis->Y + vec->Y);
		}

		inline static Vector2^ operator-(Vector2^ dis) {
			return gcnew Vector2(-dis->X, -dis->Y);
		}
		inline static Vector2^ operator-(Vector2^ dis, Vector2^ vec) {
			return gcnew Vector2(dis->X - vec->X, dis->Y - vec->Y);
		}

		inline static Vector2^ operator*(Vector2^ dis, Vector2^ vec) {
			return gcnew Vector2(dis->X * vec->X, dis->Y * vec->Y);
		}
		inline static Vector2^ operator*(Vector2^ dis, T value) {
			return gcnew Vector2(dis->X * value, dis->Y * value);
		}

		inline static Vector2^ operator/(Vector2^ dis, T value) {
			return gcnew Vector2(dis->X / value, dis->Y / value);
		}
		inline static Vector2^ operator/(Vector2^ dis, Vector2^ vec) {
			return gcnew Vector2(dis->X / vec->X, dis->Y / vec->Y);
		}

		inline static bool operator==(Vector2^ dis, Vector2^ vec) {
			return dis->X == vec->X && dis->Y == vec->Y;
		}
		inline static bool operator!=(Vector2^ dis, Vector2^ vec) {
			return !operator==(dis, vec);
		}

		inline T GetLength() {
			return instance->GetLength();
		}
		inline T GetDistanceTo(Vector2^ vec) {
			return instance->GetDistanceTo(*vec->instance);
		}
		inline T Dot(Vector2^ vec) {
			return instance->Dot(*vec->instance);
		}

		Vector2^ GetNormalized() {
			T length = GetLength();

			return gcnew Vector2(X / length, Y / length);
		}
		void Normalize() {
			instance->Normalize();
		}

		property T X {
	public:
		T get() {
			return instance->X;
		}
		void set(T value) {
			instance->X = safe_cast<T>(value);
		}
		}
		property T Y {
	public:
		T get() {
			return instance->Y;
		}
		void set(T value) {
			instance->Y = safe_cast<T>(value);
		}
		}

		System::String^ ToString() override {
			return "{ " + X + "; " + Y + " }";
		}

	};
#undef Vector2
#undef T

}
