#pragma once

#include <OSKengine/Vector4.hpp>

#include "Wrapper.h"

#include "Vector2d.h"
#include "Vector2f.h"
#include "Vector2i.h"
#include "Vector2ui.h"

namespace OSKengine {

#define Vector4 Vector4f
#define Vector2 Vector2f
#define T float

	public ref class Vector4 : Wrapper<OSK::Vector4> {

	public:

		Vector4() : Wrapper(new OSK::Vector4()) {

		}
		Vector4(T x, T y, T z, T w) : Wrapper(new OSK::Vector4(x, y, z, w)) {

		}
		Vector4(T value) : Wrapper(new OSK::Vector4(value)) {

		}

		inline static Vector4^ operator+(Vector4^ dis, Vector4^ vec) {
			return gcnew Vector4(dis->instance->operator+(*vec->instance));
		}
		
		inline static Vector4^ operator-(Vector4^ dis) {
			return gcnew Vector4(dis->instance->operator-());
		}
		inline static Vector4^ operator-(Vector4^ dis, Vector4^ vec) {
			return gcnew Vector4(dis->instance->operator-(*vec->instance));
		}

		inline static Vector4^ operator*(Vector4^ dis, Vector4^ vec) {
			return gcnew Vector4(dis->instance->operator*(*vec->instance));
		}
		inline static Vector4^ operator*(Vector4^ dis, T val) {
			return gcnew Vector4(dis->instance->operator*(val));
		}
		
		inline static Vector4^ operator/(Vector4^ dis, Vector4^ vec) {
			return gcnew Vector4(dis->instance->operator/(*vec->instance));
		}
		inline static Vector4^ operator/(Vector4^ dis, T val) {
			return gcnew Vector4(dis->instance->operator/(val));
		}

		inline static bool operator==(Vector4^ dis, Vector4^ other) {
			return dis->instance->operator==(*other->instance);
		}
		inline static bool operator!=(Vector4^ dis, Vector4^ other) {
			return dis->instance->operator!=(*other->instance);
		}

		inline Vector2^ GetRectanglePosition() {
			Vector2^ vec = gcnew Vector2();
			//*vec->instance = instance->GetRectanglePosition();

			return vec;
		}

	private:

		Vector4(OSK::Vector4 vec) : Wrapper(new OSK::Vector4()) {
			this->SetNativeValue(vec);
		}

	};

#undef Vector4
#undef Vector2
#undef T

}