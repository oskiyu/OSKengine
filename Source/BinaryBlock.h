#pragma once

#include "ApiCall.h"
#include "Assert.h"

#include "DynamicArray.hpp"
#include "BinaryExceptions.h"

#include <string>


namespace OSK::PERSISTENCE {

	/// @brief Lector de un bloque binario.
	class OSKAPI_CALL BinaryBlockReader {

	public:

		/// @brief Crea un lector sobre los datos indicados.
		/// @param data Datos a leer.
		/// @pre El contenedor a partir del cual se ha creado @p data
		/// debe ser estable.
		explicit BinaryBlockReader(std::span<const TByte> data);

		/// @brief Crea un lector sobre los datos indicados.
		/// @param data Datos a leer.
		/// @param offset Offset inicial.
		/// @pre El contenedor a partir del cual se ha creado @p data
		/// debe ser estable.
		BinaryBlockReader(std::span<const TByte> data, UIndex64 offset);


		/// @brief Lee un dato del bloque.
		/// Avanza el cursor.
		/// @tparam T Tipo de dato.
		/// @return Dato leído.
		template <typename T>
		T Read() {
			OSK_ASSERT(m_index + sizeof(T) <= m_data.size(), FinishedBlockReaderException{});

			const TByte* data = &m_data[m_index];
			m_index += sizeof(T);

			return *reinterpret_cast<const T*>(data);
		}

		/// @brief Lee un string.
		/// Avanza el cursor.
		/// @return String leído.
		std::string ReadString();

		/// @return Posición actual del cursor.
		UIndex64 GetCurrentIndex() const;

		/// @return Offset original del reader.
		UIndex64 GetOriginalOffset() const;

	private:

		UIndex64 m_index = 0;
		UIndex64 m_originalIndex = 0;
		std::span<const TByte> m_data;

	};


	/// @brief Bloque con datos en formato binario.
	class OSKAPI_CALL BinaryBlock {

	public:

		/// @return Bloque vacío.
		static BinaryBlock Empty();

		/// @param data Datos iniciales del bloque.
		/// @return Bloque con los datos indicados.
		static BinaryBlock FromData(const DynamicArray<TByte>& data);


		/// @brief Escribe información en el bloque.
		/// @tparam T Tipo de dato.
		/// @param data Datos a almacenar.
		template <typename T>
		void Write(const T& data) {
			for (UIndex64 i = 0; i < sizeof(T); i++) {
				m_data.Insert(reinterpret_cast<const TByte*>(&data)[i]);
			}
		}

		/// @brief Escribe un string en el bloque.
		/// @param string String a escribir.
		/// @note El string que se guarda es null-terminated.
		void WriteString(std::string_view string);

		/// @brief Añade los datos del bloque a este bloque.
		/// @param block Bloque a añadir.
		void AppendBlock(const BinaryBlock& block);


		/// @return Tamaño actual del bloque, en bytes.
		USize64 GetCurrentSize() const;

		/// @return Datos almacenados.
		std::span<const TByte> GetData() const;


		/// @return Bloque lector, que ocupa todo el bloque.
		/// @pre El bloque @p this debe ser estable.
		BinaryBlockReader GetReader() const;

		/// @param offset Offset inicial.
		/// @return Bloque lector, que ocupa el bloque a partir de la
		/// posición @p offset.
		/// @pre El bloque @p this debe ser estable.
		BinaryBlockReader GetReader_WithOffset(USize64 offset) const;

	private:

		DynamicArray<TByte> m_data;

	};

}
