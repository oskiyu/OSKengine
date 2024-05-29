#pragma once

/// @brief Deshabilita la copia de la clase @p className.
#define OSK_DISABLE_COPY(className) \
className(const className&) noexcept = delete; \
className& operator=(const className&) noexcept = delete;

/// @brief Deshabilita el movimiento de la clase @p className.
#define OSK_DISABLE_MOVE(className) \
className(className&&) noexcept = delete; \
className& operator=(className&&) noexcept = delete;

/// @brief Establece la copia por defecto de la clase @p className.
#define OSK_DEFAULT_COPY_OPERATOR(className) \
className(const className&) noexcept = default; \
className& operator=(const className&) noexcept = default;

/// @brief Establece el movimiento por defecto de la clase @p className.
#define OSK_DEFAULT_MOVE_OPERATOR(className) \
className(className&&) noexcept = default; \
className& operator=(className&&) noexcept = default;
