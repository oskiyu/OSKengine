#pragma once

#include "OSKmacros.h"
#include "HashMap.hpp"

#include <shared_mutex>


namespace OSK {

	/// @brief Clase que mantiene un conteo del número de
	/// trabajos de un tipo o tag dado que están siendo ejecutados
	/// en un momento dado.
	/// 
	/// @threadsafe
	class OSKAPI_CALL JobCountMap {

	public:

		/// @brief Incrementa en 1 el contador para el tipo
		/// o tag dado.
		/// 
		/// Si el tipo o tag no estaba previamente registrado,
		/// lo registra.
		/// @param tag Tag a incrementar.
		/// 
		/// @threadsafe
		void Increment(std::string_view tag);

		/// @brief Decrementa en 1 el contador para el tipo
		/// o tag dado.
		/// @param tag Tag a decrementar.
		/// 
		/// @pre El tag @p tag debe haber sido previamente registrado.
		/// @pre El valor para @p tag debe ser mayor que 0.
		/// 
		/// @threadsafe
		void Decrement(std::string_view tag);


		/// @param tag Tag a obtener.
		/// @return Valor del contador del tag.
		/// 
		/// @threadsafe
		USize32 GetCount(std::string_view tag) const;

	private:

		std::unordered_map<std::string, std::atomic<USize32>, StringHasher, std::equal_to<>> m_map;
		mutable std::shared_mutex m_mutex;

	};

}
