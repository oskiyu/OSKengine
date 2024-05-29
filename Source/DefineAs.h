#pragma once

// Para std::is_base_of_v
#include <type_traits>

/// @brief Define funciones de utilidad para clases
/// base e interfaces.
/// Incluye:
/// - As<>(): permite castear la clase a un tipo derivado.
/// - As<>() const: permite castear la clase a un tipo derivado.
/// - Is<>() const: permite conocer si una clase es de un tipo derivado.
/// 
/// @param className Tipo de la clase.
#define OSK_DEFINE_AS(className) \
template <typename T> constexpr const T* As() const requires std::is_base_of_v<className, T> { return static_cast<const T*>(this); } \
template <typename T> constexpr T* As() requires std::is_base_of_v<className, T> { return static_cast<T*>(this); } \
template <typename T> constexpr bool Is() const requires std::is_base_of_v<className, T> { return static_cast<bool>(dynamic_cast<const T*>(this)); }
