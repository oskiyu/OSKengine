#include "OSKmacros.h"

#include "Concepts.h"

#include <array>

namespace OSK {

	template <typename T, USize64 MaxSize> requires Concepts::IsCopyable<T> || Concepts::IsMovable<T>
	class StaticArray {

	public:

		StaticArray() = default;
		explicit(false) StaticArray(const std::initializer_list<T>& list) requires Concepts::IsCopyable<T> : m_count(list.size()) {
			for (UIndex64 i = 0, auto it = list.begin(); i < list.size(); i++, ++it) {
				m_data[i] = *it;
			}
		}

		StaticArray(const StaticArray& other) requires Concepts::IsCopyable<T> : m_count(other.m_count) {
			if constexpr (::std::is_trivially_copyable_v<T>)
		}

		USize64 GetCount() const {
			return m_count;
		}

	private:

		std::array<T, MaxSize> m_data;
		USize64 m_count;

	};

}
