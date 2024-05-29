#pragma once

namespace OSK {

	/// <summary>
	/// Pareja con dos elementos.
	/// </summary>
	/// <typeparam name="T1">Tipo del primer elemento.</typeparam>
	/// <typeparam name="T2">Tipo del segundo elemento.</typeparam>
	template <typename T1, typename T2> struct Pair {

		Pair(const T1& first, const T2& second) : first(first), second(second) {

		}

		Pair(const T1& first, T2&& second) : first(first), second(std::move(second)) {

		}

		bool operator==(const Pair& other) const {
			return first == other.first && second == other.second;
		}

		T1 first;
		T2 second;

	};

}
