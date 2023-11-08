#pragma once

#include "OSKmacros.h"

namespace OSK {

	template <typename T>
	class Span {

	public:

		Span(T* data, USize64 count) : m_data(data), m_count(count) {}

		Span(const Span&) = default;
		Span& operator=(const Span&) = default;

		Span(const Span&& other) : m_data(other.m_data), m_count(other.m_count) {
			if (&other == this)
				return *this;

			other.m_data = nullptr;
			other.m_count = 0;

			return *this;
		}
		Span& operator=(const Span&) = default;

		~Span() = default;

		// --- Getters --- //

		const T& At(UIndex64 index) const {
			return m_data[index];
		}

		T& At(UIndex64 index) {
			return m_data[index];
		}

		const T& operator[] (UIndex64 index) const {
			return m_data[index];
		}

		T& operator[] (UIndex64 index) {
			return m_data[index];
		}

		USize64 GetCount() const {
			return m_count;
		}

		T* GetData() {
			return m_data;
		}

		const T* GetData() const {
			return m_data;
		}

	private:

		T* m_data = nullptr;
		USize64 m_count = 0;

	};

}
