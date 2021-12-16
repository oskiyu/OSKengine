#include <glm.hpp>

export module OSKengine.Types.Vector2;

export namespace OSK {

	template <typename T> class Vector2_t {

	public:

		Vector2_t() {
			X = 0;
			Y = 0;
		}

		Vector2_t(T x, T y) {
			X = x;
			Y = y;
		}

		Vector2_t(T value) {
			X = value;
			Y = value;
		}

		Vector2_t(const glm::vec2& vec) {
			X = vec.x;
			Y = vec.y;
		}

		inline Vector2_t operator+(const Vector2_t& vec) const {
			return Vector2_t(X + vec.X, Y + vec.Y);
		}
		inline Vector2_t& operator+=(const Vector2_t& vec) {
			X += vec.X;
			Y += vec.Y;

			return *this;
		}

		inline Vector2_t operator-() const {
			return Vector2_t(-X, -Y);
		}
		inline Vector2_t operator-(const Vector2_t& vec) const {
			return Vector2_t(X - vec.X, Y - vec.Y);
		}
		inline Vector2_t& operator-=(const Vector2_t& vec) {
			X -= vec.X;
			Y -= vec.Y;

			return *this;
		}

		inline Vector2_t operator*(const Vector2_t& vec) const {
			return Vector2_t(X * vec.X, Y * vec.Y);
		}
		inline Vector2_t& operator*=(const Vector2_t& vec) {
			X *= vec.X;
			Y *= vec.Y;

			return *this;
		}
		inline Vector2_t operator*(T value) const {
			return Vector2_t(X * value, Y * value);
		}
		inline Vector2_t& operator*=(T value) {
			X *= value;
			Y *= value;

			return *this;
		}

		inline Vector2_t operator/(T value) const {
			return Vector2_t(X / value, Y / value);
		}
		inline Vector2_t& operator/=(const Vector2_t& vec) {
			X /= vec.X;
			Y /= vec.Y;

			return *this;
		}
		inline Vector2_t operator/(const Vector2_t& vec) const {
			return Vector2(X / vec.X, Y / vec.Y);
		}
		inline Vector2_t& operator/=(const T& value) {
			X /= value;
			Y /= value;

			return *this;
		}

		bool operator==(const Vector2_t& vec) const {
			return X == vec.X && Y == vec.Y;
		}
		bool operator!=(const Vector2_t& vec) const {
			return !operator==(vec);
		}

		inline T GetLength() const {
			return glm::sqrt(X * X + Y * Y);
		}
		inline T GetDistanceTo(const Vector2_t& vec) const {
			T dx = X - vec.X;
			T dy = Y - vec.Y;

			return glm::sqrt(dx * dx + dy * dy);
		}

		inline T Dot(const Vector2_t& vec) const {
			return X * vec.X + Y * vec.Y;
		}

		Vector2_t GetNormalized() const {
			T length = GetLength();

			return Vector2_t(X / length, Y / length);
		}
		void Normalize() {
			T length = GetLength();

			X /= length;
			Y /= length;
		}

		inline glm::vec2 ToGLM() const {
			return glm::vec2(X, Y);
		}
		inline glm::vec<2, T> ToGLM_T() const {
			return glm::vec<2, T>(X, Y);
		}

		template <typename P> inline Vector2_t<P> ToVec2() const {
			return Vector2_t<P>((P)X, (P)Y);
		}

		inline Vector2_t<float_t> ToVector2f() const {
			return ToVec2<float>();
		}
		inline Vector2_t<double_t> ToVector2d() const {
			return ToVec2<double_t>();
		}
		inline Vector2_t<int32_t> ToVector2i() const {
			return ToVec2<int32_t>();
		}
		inline Vector2_t<uint32_t> ToVector2ui() const {
			return ToVec2<uint32_t>();
		}

		T X;
		T Y;

	};

	typedef Vector2_t<float_t> Vector2;
	typedef Vector2_t<float_t> Vector2f;
	typedef Vector2_t<double_t> Vector2d;
	typedef Vector2_t<int32_t> Vector2i;
	typedef Vector2_t<uint32_t> Vector2ui;

}
