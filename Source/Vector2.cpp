#include "Vector2.hpp"

using namespace OSK;

template<> Vector2i const Vector2i::Zero = Vector2i(0);
template<> Vector2ui const Vector2ui::Zero = Vector2ui(0u);
template<> Vector2f const Vector2f::Zero = Vector2f(0.0f);
template<> Vector2d const Vector2d::Zero = Vector2d(0.0);

template<> Vector2i const Vector2i::One = Vector2i(1);
template<> Vector2ui const Vector2ui::One = Vector2ui(1u);
template<> Vector2f const Vector2f::One = Vector2f(1.0f);
template<> Vector2d const Vector2d::One = Vector2d(1.0);
