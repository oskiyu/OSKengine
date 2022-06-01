#pragma once

#include <type_traits>

namespace OSK::MATH {

	/// <summary>
	/// Concepto que permite distinguir cualquier tipo num�rico.
	/// @note Tipos de coma flotante: `float`, `double`, `long double`.
	/// @note Tipos enteros: `bool`, `char`, `unsigned char`, `float`, `short`,
	/// `unsigned short`, `int`, `unsigned int`, `long`, `unsigned long`, `long long`,
	/// `unsigned long long`.
	/// </summary>
	template <typename T> concept IsNumeric = std::is_floating_point_v<T> && std::is_integral_v<T>;

	/// <summary>
	/// Concepto que permite distinguir cualquier tipo num�rico entero.
	/// @note Tipos enteros: `bool`, `char`, `unsigned char`, `float`, `short`,
	/// `unsigned short`, `int`, `unsigned int`, `long`, `unsigned long`, `long long`,
	/// `unsigned long long`.
	/// </summary>
	template <typename T> concept IsInteger = std::is_integral_v<T>;

	/// <summary>
	/// Devuelve el primer m�ltiplo de `multiplo` que sea igual o mayor que �n�mero�.
	/// </summary>
	/// <typeparam name="T">Tipo de dato.</typeparam>
	/// <param name="numero">N�mero por el que se empieza la b�squeda.</param>
	/// <param name="multiplo">M�ltiplo buscado.</param>
	/// <returns>M�ltiplo mayor o igual que el n�mero.</returns>
	/// 
	/// @pre El tipo de dato debe cumplir IsInteger.
	template <typename T> inline T PrimerMultiploSuperior(T numero, T multiplo) {
		T resto = numero % multiplo;
		if (numero % multiplo == 0)
			return numero;

		return (numero / multiplo + 1) * multiplo;
	}

	/// <summary>
	/// Devuelve el primer m�ltiplo de `multiplo` que sea igual o menor que �n�mero�.
	/// </summary>
	/// <typeparam name="T">Tipo de dato.</typeparam>
	/// <param name="numero">N�mero por el que se empieza la b�squeda.</param>
	/// <param name="multiplo">M�ltiplo buscado.</param>
	/// <returns>M�ltiplo menor o igual que el n�mero.</returns>
	/// 
	/// @pre El tipo de dato debe cumplir IsInteger.
	template <typename T> T PrimerMultiploInferior(T numero, T multiplo) {
		T resto = numero % multiplo;
		if (numero % multiplo == 0)
			return numero;

		return (numero / multiplo - 1) * multiplo;
	}

	/// <summary>
	/// Interpolaci�n lineal r�pida.
	/// Para cuando factor est� entre 0.0 y 1.0
	/// </summary>
	/// <param name="a">Valor devuelto cuando factor sea 0.0</param>
	/// <param name="b">Valor devuelto cuando factor sea 1.0</param>
	/// <param name="factor">Factor que permite elegir la interpolaci�n entre a y b</param>
	/// <returns>Valor linealmente interpolado.</returns>
	/// 
	/// @warning Si factor est� fuera del rango 0-1, el valor devuelto no estar�
	/// entre a y b.
	/// 
	/// @pre El tipo T debe tener definidos los operadores -, + y *.
	template <typename T> inline T LinearInterpolation_Fast(const T& a, const T& b, float factor) {
		return (a * (1.0f - factor)) + (b * factor);
	}

	/// <summary>
	/// Interpolaci�n lineal r�pida.
	/// </summary>
	/// <param name="a">Valor devuelto cuando factor sea <= 0.0</param>
	/// <param name="b">Valor devuelto cuando factor sea >= 1.0</param>
	/// <param name="factor">Factor que permite elegir la interpolaci�n entre a y b</param>
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
	/// Devuelve el valor, ajust�ndolo de manera que nunca sea mayor que
	/// el m�ximo ni menor que el m�nimo.
	/// </summary>
	/// <param name="value">Valor al que se le aplicar� los l�mites.</param>
	/// <param name="min">L�mite inferior.</param>
	/// <param name="max">L�mite superior.</param>
	/// 
	/// @pre El tipo T debe tener definidos los operadores > y <.
	template <typename T> inline T Clamp(const T& value, const T& min, const T& max) {
		if (value > max)
			return max;

		if (value < min)
			return min;

		return value;
	}

}
