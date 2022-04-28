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

}
