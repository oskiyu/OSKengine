#pragma once

#include <OSKengine/Vector3.hpp>
#include <OSKengine/ToString.h>

#include "Wrapper.h"

namespace OSKengine {

#define Vector3 Vector3i
#define T int

	public ref class Vector3 : Wrapper<OSK::Vector3_t<T>> {

	public:

		Vector3() : Wrapper(new OSK::Vector3()) {

		}
		Vector3(T x, T y, T z) : Wrapper(new OSK::Vector3(x, y, z)) {

		}
		Vector3(T value) : Wrapper(new OSK::Vector3(value)) {

		}

		inline static Vector3^ operator+(Vector3^ dis, Vector3^ other) {
			return gcnew Vector3(dis->X + other->X, dis->Y + other->Y, dis->Z + other->Z);
		}

		inline static Vector3^ operator-(Vector3^ dis) {
			return gcnew Vector3(-dis->X, -dis->Y, -dis->Z);
		}
		inline static Vector3^ operator-(Vector3^ dis, Vector3^ other) {
			return gcnew Vector3(dis->X - other->X, dis->Y - other->Y, dis->Z - other->Z);
		}

		inline static Vector3^ operator*(Vector3^ dis, Vector3^ other) {
			return gcnew Vector3(dis->X * other->X, dis->Y * other->Y, dis->Z * other->Z);
		}
		inline static Vector3^ operator*(Vector3^ dis, T other) {
			return gcnew Vector3(dis->X * other, dis->Y * other, dis->Z * other);
		}

		inline static Vector3^ operator/(Vector3^ dis, Vector3^ other) {
			return gcnew Vector3(dis->X / other->X, dis->Y / other->Y, dis->Z / other->Z);
		}
		inline static Vector3^ operator/(Vector3^ dis, T other) {
			return gcnew Vector3(dis->X / other, dis->Y / other, dis->Z / other);
		}

		inline static bool operator==(Vector3^ dis, Vector3^ other) {
			return dis->instance->operator==(*other->instance);
		}
		inline static bool operator!=(Vector3^ dis, Vector3^ other) {
			return dis->instance->operator!=(*other->instance);
		}

		inline T GetLength() {
			return instance->GetLenght();
		}
		inline T GetDistanceTo(Vector3^ vec) {
			return instance->GetDistanceTo(*vec->instance);
		}
		inline T GetDistanceTo2(Vector3^ vec) {
			return instance->GetDistanceTo2(*vec->instance);
		}

		inline T Dot(Vector3^ vec) {
			return instance->Dot(*vec->instance);
		}
		inline Vector3^ Cross(Vector3^ vec) {
			OSK::Vector3 vec_n = instance->Cross(*vec->instance);
			Vector3^ nVec = gcnew Vector3();
			*nVec->instance = vec_n;

			return nVec;
		}

		inline void Normalize() {
			instance->Normalize();
		}
		inline Vector3^ GetNormalized() {
			OSK::Vector3 vec_n = instance->GetNormalized();
			Vector3^ nVec = gcnew Vector3();
			*nVec->instance = vec_n;

			return nVec;
		}

		System::String^ ToString() override {
			return gcnew System::String(OSK::ToString(*instance).c_str());
		}

		property T X {
	public:
		T get() {
			return instance->X;
		}
		void set(T val) {
			instance->X = val;
		}
		}
		property T Y {
	public:
		T get() {
			return instance->Y;
		}
		void set(T val) {
			instance->Y = val;
		}
		}
		property T Z {
	public:
		T get() {
			return instance->Z;
		}
		void set(T val) {
			instance->Z = val;
		}
		}

	};

#undef Vector3
#undef T

}