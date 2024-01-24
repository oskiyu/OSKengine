#pragma once

#include "OSKmacros.h"

#include <concepts>

namespace OSK::Concepts {
	
	template <typename T>
	concept IsCopyable = ::std::copyable<T> || ::std::is_copy_assignable_v<T>;

	template <typename T>
	concept IsMovable = ::std::movable<T> || ::std::is_move_assignable_v<T>;

}
