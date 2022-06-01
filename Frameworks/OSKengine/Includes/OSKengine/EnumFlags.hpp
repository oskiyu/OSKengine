#pragma once

#include <type_traits>

/// <summary>
/// Permite la manipulación de enum class como flags de un enum.
/// </summary>
namespace OSK::EFTraits {
	
	/// <summary>
	/// @note Por defecto, cualquier tipo NO se considera un enum flags,
	/// al no ser que se especifique lo contrario con OSK_FLAGS.
	/// </summary>
	/// <typeparam name="T">Tipo de dato.</typeparam>
	/// <returns>Falso, al no ser que se sobreescriba con OSK_FLAGS.</returns>
	template <typename T> constexpr bool IsEnumFlag(T) {
		return false;
	}

	/// <summary>
	/// Concepto para que los operadores sólo funcionen con enum flags.
	/// </summary>
	template <typename T> concept IsEnum = IsEnumFlag<T>({});

}


template <typename TEnum> constexpr TEnum operator&(TEnum left, TEnum right) requires OSK::EFTraits::IsEnum<TEnum> {
	using TUType = std::underlying_type_t<TEnum>;

	return static_cast<TEnum>(
		static_cast<TUType>(left) &
		static_cast<TUType>(right)
		);
}

template <typename T> constexpr T operator&=(T& left, T right) requires OSK::EFTraits::IsEnum<T> {
	left = left & right;

	return left;
}

template <typename TEnum> constexpr TEnum operator|(TEnum left, TEnum right) requires OSK::EFTraits::IsEnum<TEnum> {
	using TUType = std::underlying_type_t<TEnum>;

	return static_cast<TEnum>(
		static_cast<TUType>(left) |
		static_cast<TUType>(right)
		);
}

template <typename T> constexpr T operator|=(T& left, T right) requires OSK::EFTraits::IsEnum<T> {
	left = left | right;

	return left;
}

template <typename TEnum> constexpr TEnum operator~(TEnum left) requires OSK::EFTraits::IsEnum<TEnum> {
	using TUType = std::underlying_type_t<TEnum>;

	return static_cast<TEnum>(
		~static_cast<TUType>(left)
		);
}

#define OSK_FLAGS(T) template <> constexpr bool OSK::EFTraits::IsEnumFlag<T>(T) { return true; };

namespace OSK::EFTraits {

	/// <summary>
	/// Devuelve true si el enum 'value' tiene la flag 'flag'.
	/// </summary>
	/// <param name="value">Enum a comrpobar.</param>
	/// <param name="flag">Flag que el enum debe tener para devolver true.</param>
	template <typename TEnum> constexpr bool HasFlag(TEnum value, TEnum flag) requires OSK::EFTraits::IsEnum<TEnum> {
		using TUType = std::underlying_type_t<TEnum>;

		return static_cast<TUType>((TEnum)value & (TEnum)flag) != 0;
	}

	/// <summary>
	/// Añade un flag al enum dado.
	/// </summary>
	/// <param name="value">Puntero al enum que se va a modificar.</param>
	/// <param name="flag">Flag que se va a añadir.</param>
	template <typename TEnum> constexpr void AddFlag(TEnum* value, TEnum flag) requires OSK::EFTraits::IsEnum<TEnum> {
		using TUType = std::underlying_type_t<TEnum>;

		*value |= flag;
	}

	/// <summary>
	/// Elimina un flag del enum dado.
	/// </summary>
	/// <param name="value">Puntero al enum que se va a modificar.</param>
	/// <param name="flag">Flag que se va a eliminar.</param>
	template <typename TEnum> constexpr void RemoveFlag(TEnum* value, TEnum flag) requires OSK::EFTraits::IsEnum<TEnum> {
		using TUType = std::underlying_type_t<TEnum>;

		*value &= ~flag;
	}

}
