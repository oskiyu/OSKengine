#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <type_traits>

#include "Serializer.h"

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Quaternion.h"

namespace OSK::MATH {

	/// <summary>
	/// Concepto que permite distinguir cualquier tipo numérico.
	/// @note Tipos de coma flotante: `float`, `double`, `long double`.
	/// @note Tipos enteros: `bool`, `char`, `unsigned char`, `float`, `short`,
	/// `unsigned short`, `int`, `unsigned int`, `long`, `unsigned long`, `long long`,
	/// `unsigned long long`.
	/// </summary>
	template <typename T> concept IsNumeric = std::is_floating_point_v<T> && std::is_integral_v<T>;

	/// <summary>
	/// Concepto que permite distinguir cualquier tipo numérico entero.
	/// @note Tipos enteros: `bool`, `char`, `unsigned char`, `float`, `short`,
	/// `unsigned short`, `int`, `unsigned int`, `long`, `unsigned long`, `long long`,
	/// `unsigned long long`.
	/// </summary>
	template <typename T> concept IsInteger = std::is_integral_v<T>;

	/// <summary>
	/// Devuelve el primer múltiplo de `multiplo` que sea igual o mayor que ´número´.
	/// </summary>
	/// <typeparam name="T">Tipo de dato.</typeparam>
	/// <param name="numero">Número por el que se empieza la búsqueda.</param>
	/// <param name="multiplo">Múltiplo buscado.</param>
	/// <returns>Múltiplo mayor o igual que el número.</returns>
	/// 
	/// @pre El tipo de dato debe cumplir IsInteger.
	template <typename T> inline T PrimerMultiploSuperior(T numero, T multiplo) {
		T resto = numero % multiplo;
		if (numero % multiplo == 0)
			return numero;

		return (numero / multiplo + 1) * multiplo;
	}

	/// <summary>
	/// Devuelve el primer múltiplo de `multiplo` que sea igual o menor que ´número´.
	/// </summary>
	/// <typeparam name="T">Tipo de dato.</typeparam>
	/// <param name="numero">Número por el que se empieza la búsqueda.</param>
	/// <param name="multiplo">Múltiplo buscado.</param>
	/// <returns>Múltiplo menor o igual que el número.</returns>
	/// 
	/// @pre El tipo de dato debe cumplir IsInteger.
	template <typename T> T PrimerMultiploInferior(T numero, T multiplo) {
		T resto = numero % multiplo;
		if (numero % multiplo == 0)
			return numero;

		return (numero / multiplo - 1) * multiplo;
	}

	/// <summary>
	/// Interpolación lineal rápida.
	/// Para cuando factor está entre 0.0 y 1.0
	/// </summary>
	/// <param name="a">Valor devuelto cuando factor sea 0.0</param>
	/// <param name="b">Valor devuelto cuando factor sea 1.0</param>
	/// <param name="factor">Factor que permite elegir la interpolación entre a y b</param>
	/// <returns>Valor linealmente interpolado.</returns>
	/// 
	/// @warning Si factor está fuera del rango 0-1, el valor devuelto no estará
	/// entre a y b.
	/// 
	/// @pre El tipo T debe tener definidos los operadores -, + y *.
	template <typename T> inline T LinearInterpolation_Fast(const T& a, const T& b, float factor) {
		return (a * (1.0f - factor)) + (b * factor);
	}

	/// <summary>
	/// Interpolación lineal rápida.
	/// </summary>
	/// <param name="a">Valor devuelto cuando factor sea <= 0.0</param>
	/// <param name="b">Valor devuelto cuando factor sea >= 1.0</param>
	/// <param name="factor">Factor que permite elegir la interpolación entre a y b</param>
	/// <returns>Valor linealmente interpolado.</returns>
	/// 
	/// @pre El tipo T debe tener definidos los operadores >=, <=, -, + y *.
	template <typename T> inline T LinearInterpolation(const T& a, const T& b, float factor) {
		if (factor >= 1.0f)
			return b;

		if (factor <= 0.0f)
			return a;

		return (a * (1.0f - factor)) + (b * factor);
	}

	/// <summary>
	/// Devuelve el valor, ajustándolo de manera que nunca sea mayor que
	/// el máximo ni menor que el mínimo.
	/// </summary>
	/// <param name="value">Valor al que se le aplicará los límites.</param>
	/// <param name="min">Límite inferior.</param>
	/// <param name="max">Límite superior.</param>
	/// 
	/// @pre El tipo T debe tener definidos los operadores > y <.
	template <typename T> inline T Clamp(const T& value, const T& min, const T& max) {
		if (value > max)
			return max;

		if (value < min)
			return min;

		return value;
	}

	/// <summary>
	/// Para dos matrices modelo, devuelve una matriz con la diferencia de posición, rotación y escala entra ambas.
	/// </summary>
	/// <param name="first">Matriz modelo A.</param>
	/// <param name="second">Matriz modelo B.</param>
	/// <returns>Offset de B respecto a A (A - B).</returns>
	static inline glm::mat4 GetMatrixOffset(const glm::mat4& first, const glm::mat4& second) {
		return glm::inverse(first) - second;
	}

	template <typename T> T Sign(T value) {
		return value >= static_cast<T>(0) ? static_cast<T>(1) : static_cast<T>(-1);
	}

}


namespace OSK::PERSISTENCE {

	template <>
	nlohmann::json inline SerializeData<glm::mat3>(const glm::mat3& data) {
		nlohmann::json output{};

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				output[std::to_string(i) + std::to_string(j)] = data[i][j];
			}
		}

		return output;
	}

	template <>
	glm::mat3 inline DeserializeData<glm::mat3>(const nlohmann::json& json) {
		glm::mat3 output{};

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				output[i][j] = json[std::to_string(i) + std::to_string(j)];
			}
		}

		return output;
	}

	template<>
	PERSISTENCE::BinaryBlock inline BinarySerializeData<glm::mat3>(const glm::mat3& data) {
		auto output = BinaryBlock::Empty();

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				output.Write(data[i][j]);
			}
		}

		return output;
	}

	template<>
	glm::mat3 inline BinaryDeserializeData<glm::mat3>(BinaryBlockReader* data) {
		glm::mat3 output{};

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				output[i][j] = data->Read<float>();
			}
		}

		return output;
	}


	template <>
	nlohmann::json inline SerializeData<glm::mat4>(const glm::mat4& data) {
		nlohmann::json output{};

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				output[std::to_string(i) + std::to_string(j)] = data[i][j];
			}
		}

		return output;
	}

	template <>
	glm::mat4 inline DeserializeData<glm::mat4>(const nlohmann::json& json) {
		glm::mat4 output{};

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				output[i][j] = json[std::to_string(i) + std::to_string(j)];
			}
		}

		return output;
	}

	template<>
	PERSISTENCE::BinaryBlock inline BinarySerializeData<glm::mat4>(const glm::mat4& data) {
		auto output = BinaryBlock::Empty();

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				output.Write(data[i][j]);
			}
		}

		return output;
	}

	template<>
	glm::mat4 inline BinaryDeserializeData<glm::mat4>(BinaryBlockReader* data) {
		glm::mat4 output{};

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				output[i][j] = data->Read<float>();
			}
		}

		return output;
	}

#pragma region Vectors

	template <typename TVec>
	inline nlohmann::json SerializeVector2(const TVec& vec) {
		nlohmann::json output{};

		output["x"] = vec.x;
		output["y"] = vec.y;

		return output;
	}

	template <typename TVec>
	inline nlohmann::json SerializeVector3(const TVec& vec) {
		nlohmann::json output{};

		output["x"] = vec.x;
		output["y"] = vec.y;
		output["z"] = vec.z;

		return output;
	}

	template <typename TVec>
	inline nlohmann::json SerializeVector4(const TVec& vec) {
		nlohmann::json output{};

		output["x"] = vec.x;
		output["y"] = vec.y;
		output["z"] = vec.z;
		output["W"] = vec.W;

		return output;
	}


	template <typename TVec>
	inline TVec DeserializeVector2(const nlohmann::json& vec) {
		TVec output{};

		output.x = vec["x"];
		output.y = vec["y"];

		return output;
	}

	template <typename TVec>
	inline TVec DeserializeVector3(const nlohmann::json& vec) {
		TVec output{};

		output.x = vec["x"];
		output.y = vec["y"];
		output.z = vec["z"];

		return output;
	}

	template <typename TVec>
	inline TVec DeserializeVector4(const nlohmann::json& vec) {
		TVec output{};

		output.x = vec["x"];
		output.y = vec["y"];
		output.z = vec["z"];
		output.W = vec["W"];

		return output;
	}


	template <typename TVec>
	inline PERSISTENCE::BinaryBlock SerializeBinaryVector2(const TVec& vec) {
		PERSISTENCE::BinaryBlock output{};

		output.Write(vec.x);
		output.Write(vec.y);

		return output;
	}

	template <typename TVec>
	inline PERSISTENCE::BinaryBlock SerializeBinaryVector3(const TVec& vec) {
		PERSISTENCE::BinaryBlock output{};

		output.Write(vec.x);
		output.Write(vec.y);
		output.Write(vec.z);

		return output;
	}

	template <typename TVec>
	inline PERSISTENCE::BinaryBlock SerializeBinaryVector4(const TVec& vec) {
		PERSISTENCE::BinaryBlock output{};

		output.Write(vec.x);
		output.Write(vec.y);
		output.Write(vec.z);
		output.Write(vec.W);

		return output;
	}

	template <typename TVec, typename TNumberType>
	inline TVec DeserializeBinaryVector2(PERSISTENCE::BinaryBlockReader* vec) {
		TVec output{};

		output.x = vec->Read<TNumberType>();
		output.y = vec->Read<TNumberType>();

		return output;
	}

	template <typename TVec, typename TNumberType>
	inline TVec DeserializeBinaryVector3(PERSISTENCE::BinaryBlockReader* vec) {
		TVec output{};

		output.x = vec->Read<TNumberType>();
		output.y = vec->Read<TNumberType>();
		output.z = vec->Read<TNumberType>();

		return output;
	}

	template <typename TVec, typename TNumberType>
	inline TVec DeserializeBinaryVector4(PERSISTENCE::BinaryBlockReader* vec) {
		TVec output{};

		output.x = vec->Read<TNumberType>();
		output.y = vec->Read<TNumberType>();
		output.z = vec->Read<TNumberType>();
		output.W = vec->Read<TNumberType>();

		return output;
	}

#pragma endregion


	template <>
	nlohmann::json inline SerializeData<Quaternion>(const Quaternion& data) {
		return SerializeData<glm::mat4>(data.ToMat4());

	}

	template <>
	Quaternion inline DeserializeData<Quaternion>(const nlohmann::json& json) {
		return Quaternion{};
		const auto matrix = DeserializeData<glm::mat4>(json);
		return Quaternion::FromGlm(glm::normalize(glm::toQuat(matrix)));
	}


	template <>
	PERSISTENCE::BinaryBlock inline BinarySerializeData<Quaternion>(const Quaternion& data) {
		return BinarySerializeData<glm::mat4>(data.ToMat4());

	}

	template <>
	Quaternion inline BinaryDeserializeData<Quaternion>(PERSISTENCE::BinaryBlockReader* reader) {
		// return Quaternion{};
		const auto matrix = BinaryDeserializeData<glm::mat4>(reader);
		return Quaternion::FromGlm(glm::normalize(glm::toQuat(matrix)));
	}

}
