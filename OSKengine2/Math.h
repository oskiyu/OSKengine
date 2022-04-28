#pragma once

#include <type_traits>

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

}
