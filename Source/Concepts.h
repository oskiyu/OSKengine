#pragma once

#include <concepts>
#include <type_traits>

namespace OSK::Concepts {
	
	template <typename T>
	concept IsCopyable = ::std::copyable<T> || ::std::is_copy_assignable_v<T>;

	template <typename T>
	concept IsMovable = ::std::movable<T> || ::std::is_move_assignable_v<T>;

	template <typename T1, typename T2>
	struct AreSameTemplate_t : ::std::is_same<T1, T2> {};

	template <template<typename...> typename T, typename T1, typename T2>
	struct AreSameTemplate_t<T<T1>, T<T2>> : ::std::true_type {};

	template <typename T1, typename T2>
	concept AreSameTemplate = AreSameTemplate_t<T1, T2>::value;
}
