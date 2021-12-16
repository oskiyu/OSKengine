#include <glm.hpp>

export module OSKengine.Types.Vector3;

export namespace OSK {

	template <typename T> class Vector3_t {

	public:

		Vector3_t() {
			X = 0;
			Y = 0;
			Z = 0;
		}
		Vector3_t(T x, T y, T z) {
			X = x;
			Y = y;
			Z = z;
		}
		Vector3_t(T value) {
			X = value;
			Y = value;
			Z = value;
		}
		Vector3_t(const glm::vec3& vec) {
			X = vec.x;
			Y = vec.y;
			Z = vec.z;
		}

		Vector3_t operator+(const Vector3_t& vec) const {
			return Vector3_t(X + vec.X, Y + vec.Y, Z + vec.Z);
		}
		Vector3_t& operator+=(const Vector3_t& vec) {
			X += vec.X;
			Y += vec.Y;
			Z += vec.Z;
			return *this;
		}

		Vector3_t operator-() const {
			return Vector3_t(-X, -Y, -Z);
		}
		Vector3_t operator-(const Vector3_t& vec) const {
			return Vector3_t(X - vec.X, Y - vec.Y, Z - vec.Z);
		}
		Vector3_t& operator-=(const Vector3_t& vec) {
			X -= vec.X;
			Y -= vec.Y;
			Z -= vec.Z;

			return *this;
		}

		Vector3_t operator*(const Vector3_t& vec) const {
			return Vector3_t(X * vec.X, Y * vec.Y, Z * vec.Z);
		}
		Vector3_t& operator*=(const Vector3_t& vec) {
			X *= vec.X;
			Y *= vec.Y;
			Z *= vec.Z;

			return *this;
		}
		Vector3_t operator*(T value) const {
			return Vector3_t(X * value, Y * value, Z * value);
		}
		Vector3_t& operator*=(T value) {
			X *= value;
			Y *= value;
			Z *= value;
			return *this;
		}

		Vector3_t operator/(const Vector3_t& vec) const {
			return Vector3_t(X / vec.X, Y / vec.Y, Z / vec.Z);
		}
		Vector3_t& operator/=(const Vector3_t& vec) {
			X /= vec;
			Y /= vec;
			Z /= vec;

			return *this;
		}
		Vector3_t operator/(T value) const {
			return Vector3_t(X / value, Y / value, Z / value);
		}
		Vector3_t& operator/=(T value) {
			X /= value;
			Y /= value;
			Z /= value;

			return *this;
		}

		bool operator==(const Vector3_t& vec) const {
			return X == vec.X && Y == vec.Y && Z == vec.Z;
		}
		bool operator!=(const Vector3_t& vec) const {
			return !operator==(vec);
		}

		T GetLenght() const {
			return glm::sqrt(X * X + Y * Y + Z * Z);
		}

		T GetDistanceTo(const Vector3_t& vec) const {
			T dx = X - vec.X;
			T dy = Y - vec.Y;
			T dz = Z - vec.Z;

			return glm::sqrt(dx * dx + dy * dy + dz * dz);
		}
		T GetDistanceTo2(const Vector3_t& vec) const {
			T dx = X - vec.X;
			T dy = Y - vec.Y;
			T dz = Z - vec.Z;

			return dx * dx + dy * dy + dz * dz;
		}

		T Dot(const Vector3_t& vec) const {
			return X * vec.X + Y * vec.Y + Z * vec.Z;
		}
		Vector3_t Cross(const Vector3_t& vec) const {
			return Vector3_t(Y * vec.Z - Z * vec.Y, Z * vec.X - X * vec.Z, X * vec.Y - Y * vec.X);
		}

		Vector3_t GetNormalized() const {
			T length = GetLenght();

			return Vector3_t(X / length, Y / length, Z / length);
		}
		void Normalize() {
			T length = GetLenght();

			X /= length;
			Y /= length;
			Z /= length;
		}

		glm::vec3 ToGLM() const {
			return glm::vec3(X, Y, Z);
		}
		inline glm::vec<3, T> ToGLM_T() const {
			return glm::vec<3, T>(X, Y, Z);
		}

		template <typename P> inline Vector3_t<P> ToVec3() const {
			return Vector3_t<P>(X, Y, Z);
		}
		inline Vector3_t<float_t> ToVector3f() const {
			return ToVec3<float>();
		}
		inline Vector3_t<double_t> ToVector3d() const {
			return ToVec3<double_t>();
		}
		inline Vector3_t<int32_t> ToVector3i() const {
			return ToVec3<int32_t>();
		}
		inline Vector3_t<uint32_t> ToVector3ui() const {
			return ToVec3<uint32_t>();
		}

		T X;
		T Y;
		T Z;

	};

	typedef Vector3_t<float_t> Vector3;
	typedef Vector3_t<float_t> Vector3f;
	typedef Vector3_t<double_t> Vector3d;
	typedef Vector3_t<int32_t> Vector3i;
	typedef Vector3_t<uint32_t> Vector3ui;

}
