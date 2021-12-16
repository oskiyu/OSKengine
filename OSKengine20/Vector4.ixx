#include <glm.hpp>

export module OSKengine.Types.Vector4;

export import OSKengine.Types.Vector2;

export namespace OSK {

	template <typename T> class Vector4_t {

	public:

		Vector4_t() {
			X = (T)0;
			Y = (T)0;
			Z = (T)0;
			W = (T)0;
		}
		Vector4_t(T x, T y, T z, T w) {
			X = x;
			Y = y;
			Z = z;
			W = w;
		}
		Vector4_t(T value) {
			X = value;
			Y = value;
			Z = value;
			W = value;
		}
		Vector4_t(const glm::vec4& vec) {
			X = vec.x;
			Y = vec.y;
			Z = vec.z;
			W = vec.w;
		}

		Vector4_t operator+(const Vector4_t& vec) const {
			return Vector4_t(X + vec.X, Y + vec.Y, Z + vec.Z, W + vec.W);
		}
		Vector4_t& operator+=(const Vector4_t& vec) {
			*this = operator+(vec);

			return *this;
		}
		Vector4_t operator-() const {
			return Vector4_t(-X, -Y, -Z, -W);
		}
		Vector4_t operator-(const Vector4_t& vec) const {
			return Vector4_t(X - vec.X, Y - vec.Y, Z - vec.Z, W - vec.W);
		}
		Vector4_t& operator-=(const Vector4_t& vec) {
			*this = operator-(vec);

			return *this;
		}

		Vector4_t operator*(const Vector4_t& vec) const {
			return Vector4_t(X * vec.X, Y * vec.Y, Z * vec.Z, W * vec.W);
		}
		Vector4_t& operator*=(const Vector4_t& vec) {
			*this = operator*(vec);

			return *this;
		}
		Vector4_t operator*(T value) const {
			return Vector4_t(X * value, Y * value, Z * value, W * value);
		}
		Vector4_t& operator*=(T value) {
			*this = operator*(value);

			return *this;
		}

		Vector4_t operator/(const Vector4_t& vec) const {
			return Vector4_t(X / vec.X, Y / vec.Y, Z / vec.Z, W / vec.W);
		}
		Vector4_t& operator/=(const Vector4_t& vec) {
			*this = operator/(vec);

			return *this;
		}
		Vector4_t operator/(const T& value) const {
			return Vector4_t(X / value, Y / value, Z / value, W / value);
		}
		Vector4_t& operator/=(const T& value) {
			*this = operator/(value);

			return *this;
		}

		bool operator==(const Vector4_t& vec) const {
			return X == vec.X && Y == vec.Y && Z == vec.Z && W == vec.W;
		}
		bool operator!=(const Vector4_t& vec) const {
			return !operator==(vec);
		}

		Vector2_t<T> GetRectanglePosition() const {
			return Vector2_t<T>(X, Y);
		}
		Vector2_t<T> GetRectangleSize() const {
			return Vector2_t<T>(Z, W);
		}
		inline T GetRectangleWidth() const {
			return Z;
		}
		inline T GetRectangleHeight() const {
			return W;
		}
		inline T GetRectangleTop() const {
			return Y + W;
		}
		inline T GetRectangleRight() const {
			return X + Z;
		}
		Vector2_t<T> GetRectangleMiddlePoint() const {
			return Vector2_t<T>(X + GetRectangleWidth() / 2, Y + GetRectangleHeight() / 2);
		}
		inline bool Intersects(const Vector4_t& vec) const {
			return X < vec.X + vec.Z && X + Z > vec.X && Y < vec.Y + vec.W && W + Y > vec.Y;
		}

		inline glm::vec4 ToGLM() const {
			return glm::vec4(X, Y, Z, W);
		}
		inline glm::vec<4, T> ToGLM_T() const {
			return glm::vec<4, T>(X, Y, Z, W);
		}

		template <typename P> inline Vector4_t<P> ToVec4() const {
			return Vector4_t<P>((P)X, (P)Y, (P)Z, (P)W);
		}
		inline Vector4_t<float_t> ToVector4f() const {
			return ToVec4<float>();
		}
		inline Vector4_t<double_t> ToVector4d() const {
			return ToVec4<double_t>();
		}
		inline Vector4_t<int32_t> ToVector4i() const {
			return ToVec4<int32_t>();
		}
		inline Vector4_t<uint32_t> ToVector4ui() const {
			return ToVec4<uint32_t>();
		}

		T X;
		T Y;
		T Z;
		T W;

	};

	typedef Vector4_t<float_t> Vector4;
	typedef Vector4_t<float_t> Vector4f;
	typedef Vector4_t<double_t> Vector4d;
	typedef Vector4_t<int32_t> Vector4i;
	typedef Vector4_t<uint32_t> Vector4ui;

}
