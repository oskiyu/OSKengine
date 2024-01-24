#pragma once

#include "OSKmacros.h"
#include "DynamicArray.hpp"
#include <span>

namespace OSK::MEMORY {

	/// @brief Representa una región de memoria
	/// RAM que puede usarse para almacenar
	/// información arbitraria.
	class OSKAPI_CALL Buffer {

	public:

		/// @brief Crea un buffer vacío.
		explicit Buffer() = default;

		/// @brief Crea un buffer vacío con memoria reservada
		/// para el número de bytes indicado.
		/// Después se pordrá llenar con más información
		/// si se agota el espacio inicialmente reservado.
		/// @param initialSize Tamaño inicial, en bytes.
		explicit Buffer(USize64 initialSize);


		/// @return Tamaño de la información almacenada,
		/// en bytes.
		USize64 GetSize() const;


		/// @brief Añade datos al final del buffer.
		/// @tparam T Tipo de dato.
		/// @param data Nuevos datos.
		template <typename T>
			requires std::is_trivially_copyable_v<T>
		void AddData(const T& data) {
			AddData(&data, sizeof(data));
		}

		/// @brief Añade nuevos datos al final del buffer.
		/// @param data Nuevos datos.
		/// @param size Tamaño de los nuevos datos.
		void AddData(const void* data, USize64 size);

		/// @return Datos del buffer.
		const void* GetRawData() const;
		void* GetRawData();

		/// @brief Datos del buffer, a partir del punto indicado.
		/// @param byteOffset Offset desde el principio del buffer,
		/// en bytes.
		/// @return Datos almacenados a partir del offset.
		/// 
		/// @pre @p byteOffset < GetSize().
		void* GetRawData(UIndex64 byteOffset);
		const void* GetRawData(UIndex64 byteOffset) const;

		/// @brief Recupera un dato almacenado en el buffer.
		/// @tparam T Tipo del dato.
		/// @param byteOffset Offset del dato dentro del buffer,
		/// en bytes.
		/// @return Dato almacenado.
		/// 
		/// @pre @p byteOffset <= GetSize() + sizeof(T).
		template <typename T>
			requires std::is_trivially_copyable_v<T>
		T& GetData(UIndex64 byteOffset) const {
			return std::bit_cast<T&>(GetRawData(byteOffset));
		}

	private:

		/// @brief Datos almacenados.
		DynamicArray<TByte> m_data;

	};

}
