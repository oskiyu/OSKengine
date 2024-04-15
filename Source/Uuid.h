#pragma once

#include "OSKmacros.h"

#include <xhash>
#include <atomic>


namespace OSK {

	/// @brief Clase que representa un identificador único global.
	/// @tparam TTag Tag, que debe ser distinto para cada tipo de UUID.
	template <class TTag>
	class BaseUuid {

	public:

		/// @brief Crea un UUID vacío.
		BaseUuid() = default;

		/// @brief Crea un UUID con un valor.
		/// @param value Valor del UUID.
		explicit BaseUuid(UIndex64 value) : m_value(value) {}


		auto operator<=>(const BaseUuid&) const = default;


		/// @return Crea un UUID vacío.
		static constexpr BaseUuid CreateEmpty() {
			return BaseUuid();
		}


		/// @return True si el identificador está vacío.
		inline bool IsEmpty() const noexcept {
			return m_value == EmptyUnderlyingType;
		}

		/// @brief Establece el valor del UUID.
		/// @param value Nuevo valor.
		inline void SetValue(UIndex64 value) noexcept {
			m_value = value;
		}

		/// @return Valor del UUID.
		/// @pre IsEmpty debe devolver false.
		inline UIndex64 Get() const noexcept {
			return m_value;
		}

	private:

		constexpr static UIndex64 EmptyUnderlyingType = static_cast<UIndex64>(0);

		UIndex64 m_value = EmptyUnderlyingType;

	};


	/// @brief Clase que gestiona la asignación de UUIDs.
	class UuidProvider {

	public:

		/// @brief Genera un nuevo identificador único.
		/// @return Nuevo identificador único.
		/// @threadsafe
		UIndex64 GenerateNewUuid() {
			return m_nextIndex.fetch_add(1);
		}

	private:

		std::atomic<UIndex64> m_nextIndex = 1;

	};

}

#define OSK_DEFINE_UUID_HASH(name) \
\
\
	template<> \
	struct std::hash<name> { \
		std::size_t operator()(const name& uuid) const noexcept { \
			return std::hash<UIndex64>()(uuid.Get()); \
		} \
	}; \
 \

#define OSK_DEFINE_UUID_FORMATTER(type, text) \
template <> \
struct std::formatter<type> { \
\
	formatter() {}\
\
	constexpr auto parse(std::format_parse_context& ctx) {\
		return ctx.begin();\
	}\
\
	auto format(const type& uuid, std::format_context& ctx) const {\
		return std::format_to(ctx.out(), text, uuid.Get());\
	}\
};
