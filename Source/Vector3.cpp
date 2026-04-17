#include "Vector3.hpp"

using namespace OSK;

template<> Vector3i const Vector3i::Zero    = Vector3i (0);
template<> Vector3ui const Vector3ui::Zero  = Vector3ui(0u);
template<> Vector3f const Vector3f::Zero    = Vector3f (0.0f);
template<> Vector3d const Vector3d::Zero    = Vector3d (0.0);

template<> Vector3i  const Vector3i::One    = Vector3i (1);
template<> Vector3ui const Vector3ui::One   = Vector3ui(1u);
template<> Vector3f  const Vector3f::One    = Vector3f (1.0f);
template<> Vector3d  const Vector3d::One    = Vector3d (1.0);
