#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <limits>

namespace OSK::Math {

	/// <summary>
	/// Compara dos floats.
	/// </summary>
	/// <param name="a">float 1.</param>
	/// <param name="b">float 2.</param>
	/// <param name="epsilon">Límite de la comparación.</param>
	/// <returns>True si son comparables.</returns>
	inline bool CompareFloats(float a, float b, float epsilon = std::numeric_limits<float>::epsilon() * 2) {
		return std::abs(a - b) < epsilon;
	}

	inline float LinearInterpolation_Fast(float a, float b, float factor) {
		return (a * (1.0f - factor)) + (b * factor);
	}

	inline float LinearInterpolation(float a, float b, float factor) {
		if (factor >= 1.0f)
			return b;

		if (factor <= 0.0f)
			return a;

		return LinearInterpolation_Fast(a, b, factor);
	}

	template <typename T> inline Vector2_t<T> VectorialLinearInterpolation_Fast2(const Vector2_t<T>& a, const Vector2_t<T>& b, float factor) {
		return (a * (1.0f - factor)) + b * factor;
	}

	template <typename T> inline Vector2_t<T> VectorialLinearInterpolation2(const Vector2_t<T>& a, const Vector2_t<T>& b, float factor) {
		if (factor >= 1.0f)
			return b;

		if (factor <= 0.0f)
			return a;

		return (a * (1.0f - factor)) + b * factor;
	}

	template <typename T> inline Vector3_t<T> VectorialLinearInterpolation_Fast3(const Vector3_t<T>& a, const Vector3_t<T>& b, float factor) {
		return (a * (1.0f - factor)) + b * factor;
	}

	template <typename T> inline Vector3_t<T> VectorialLinearInterpolation3(const Vector3_t<T>& a, const Vector3_t<T>& b, float factor) {
		if (factor >= 1.0f)
			return b;

		if (factor <= 0.0f)
			return a;

		return (a * (1.0f - factor)) + b * factor;
	}

	template <typename T> inline Vector4_t<T> VectorialLinearInterpolation_Fast4(const Vector4_t<T>& a, const Vector4_t<T>& b, float factor) {
		return (a * (1.0f - factor)) + b * factor;
	}

	template <typename T> inline Vector4_t<T> VectorialLinearInterpolation4(const Vector4_t<T>& a, const Vector4_t<T>& b, float factor) {
		if (factor >= 1.0f)
			return b;

		if (factor <= 0.0f)
			return a;

		return (a * (1.0f - factor)) + b * factor;
	}

#define OSK_DEF_VEC2_LINEAR_INTERPOLATION(T, vecT) \
inline vecT LinearInterpolation(const vecT & a, const vecT & b, float factor) {return VectorialLinearInterpolation2<T>(a, b, factor);} \
inline vecT LinearInterpolation_Fast(const vecT & a, const vecT & b, float factor) { return VectorialLinearInterpolation_Fast2<T>(a, b, factor);}

	OSK_DEF_VEC2_LINEAR_INTERPOLATION(float, Vector2f);
	OSK_DEF_VEC2_LINEAR_INTERPOLATION(int, Vector2i);
	OSK_DEF_VEC2_LINEAR_INTERPOLATION(uint32_t, Vector2ui);
	OSK_DEF_VEC2_LINEAR_INTERPOLATION(double, Vector2d);

#undef OSK_DEF_VEC2_LINEAR_INTERPOLATION

#define OSK_DEF_VEC3_LINEAR_INTERPOLATION(T, vecT) \
inline vecT LinearInterpolation(const vecT & a, const vecT & b, float factor) {return VectorialLinearInterpolation3<T>(a, b, factor);} \
inline vecT LinearInterpolation_Fast(const vecT & a, const vecT & b, float factor) { return VectorialLinearInterpolation_Fast3<T>(a, b, factor);}

	OSK_DEF_VEC3_LINEAR_INTERPOLATION(float, Vector3f);
	OSK_DEF_VEC3_LINEAR_INTERPOLATION(int, Vector3i);
	OSK_DEF_VEC3_LINEAR_INTERPOLATION(uint32_t, Vector3ui);
	OSK_DEF_VEC3_LINEAR_INTERPOLATION(double, Vector3d);

#undef OSK_DEF_VEC3_LINEAR_INTERPOLATION

#define OSK_DEF_VEC4_LINEAR_INTERPOLATION(T, vecT) \
inline vecT LinearInterpolation(const vecT & a, const vecT & b, float factor) {return VectorialLinearInterpolation4<T>(a, b, factor);} \
inline vecT LinearInterpolation_Fast(const vecT & a, const vecT & b, float factor) { return VectorialLinearInterpolation_Fast4<T>(a, b, factor);}

	OSK_DEF_VEC4_LINEAR_INTERPOLATION(float, Vector4f);
	OSK_DEF_VEC4_LINEAR_INTERPOLATION(int, Vector4i);
	OSK_DEF_VEC4_LINEAR_INTERPOLATION(uint32_t, Vector4ui);
	OSK_DEF_VEC4_LINEAR_INTERPOLATION(double, Vector4d);

#undef OSK_DEF_VEC4_LINEAR_INTERPOLATION

}
