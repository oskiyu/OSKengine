#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"
#include "UniquePtr.hpp"
#include "IGpuMemorySubblock.h"

#include "VertexBufferView.h"
#include "IndexBufferView.h"

#include <optional>

namespace OSK::GRAPHICS {

	class IGpuMemoryBlock;

	/// @brief Representa un buffer que contiene información
	/// accesible desde la GPU.
	class OSKAPI_CALL GpuBuffer final {

	public:

		/// @brief Establece los parámetros del buffer.
		/// @param buffer Subbloque de memoria que tiene este buffer.
		/// @param size Tamaño del buffer. En bytes.
		/// @param alignment Alineamiento del contenido del buffer.
		/// 
		/// @note No crea el buffer, el buffer es creado por IGpuMemoryAllocator.
		explicit GpuBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);
		virtual ~GpuBuffer();

		OSK_DISABLE_COPY(GpuBuffer);

	public:

		/// @brief Establece el vertex view por defecto.
		/// @param view View por defecto.
		/// @note Se recomienda que sea un view para todo el buffer.
		void SetVertexView(const VertexBufferView& view);

		/// @brief Establece el index view por defecto.
		/// @param view View por defecto.
		/// @note Se recomienda que sea un view para todo el buffer.
		void SetIndexView(const IndexBufferView& view);


		/// @return Devuelve el vertex view por defecto.
		/// @pre Se debe haber establecido el vertex view por defecto.
		const VertexBufferView& GetVertexView() const;

		/// @return Devuelve el index view por defecto.
		/// @pre Se debe haber establecido el index view por defecto.
		const IndexBufferView& GetIndexView() const;


		/// @return True si se ha establecido el vertex view por defecto.
		bool HasVertexView() const;

		/// @return True si se ha establecido el index view por defecto.
		bool HasIndexView() const;

	public:

		/// @brief Mapea toda la región de memoria del buffer, para poder escribirla
		/// desde la CPU.
		/// 
		/// @pre Debe haber sido creado con GpuSharedMemoryType::GPU_AND_CPU.
		/// @pre No debe haber ninguna región de memoria del buffer previamente mapeada.
		void MapMemory();

		/// @brief Mapea la región de memoria dada,para poder escribirla
		/// desde la CPU
		/// 
		/// @param size Tamaño de la región, en bytes.
		/// @param offset Offset desde el inicio del buffer, en bytes.
		/// 
		/// @pre Debe haber sido creado con GpuSharedMemoryType::GPU_AND_CPU.
		/// @pre No debe haber ninguna región de memoria del buffer previamente mapeada.
		/// @pre @p size + @p offset < GetSize().
		void MapMemory(TSize size, TSize offset);

		/// @brief Escribe la información en el buffer, a partir de la posición actual
		/// del cursor. También hace avanzar el cursor.
		/// 
		/// @param data Información a escribir.
		/// @param size Tamaño de la información a escribir.
		/// 
		/// @pre La memoria debe estar mapeada.
		/// @pre @p size <= GetSize() - cursor.
		void Write(const void* data, TSize size);

		/// @brief Escribe la información del elemento dado en el buffer, usando
		/// el cursor. También hace avanzar el cursor.
		/// @tparam T Tipo de la estructura a escribir.
		/// @param data Información a escribir.
		/// 
		/// @pre La memoria debe estar mapeada.
		/// @pre @p size <= GetSize() - cursor.
		template <typename T>
		void Write(const T& data) {
			Write(&data, sizeof(T));
		}

		/// @brief Escribe la información en el buffer, sin usar el cursor.
		/// 
		/// @param data Información a escribir.
		/// @param size Número de bytes a escribir.
		/// @param offset Offset (en bytes) respecto al inicio del buffer.
		/// 
		/// @pre La memoria debe estar mapeada.
		/// @pre La información a escribir debe estar contenida dentro
		/// de la zona de memoria mapeada.
		void WriteOffset(const void* data, TSize size, TSize offset);

		/// @brief Desmapea la memoria.
		/// @pre La memoria debe estar mapeada.
		void Unmap();

		
		/// @brief Establece la posición del cursor.
		/// @param position Posición, en bytes, respecto al inicio del buffer.
		/// @pre La posición debe estar dentro de los límites del buffer.
		void SetCursor(TSize position);

		/// @brief Establece la posición del cursor a 0.
		void ResetCursor();


		/// @return Devuelve el tamaño del buffer, en bytes.
		TSize GetSize() const;

		/// @return Devuelve la alineación del buffer, en bytes.
		TSize GetAlignment() const;


		IGpuMemorySubblock* GetMemorySubblock() const;
		IGpuMemoryBlock* GetMemoryBlock() const;

	protected:

		IGpuMemorySubblock* buffer = nullptr;

	private:

		std::optional<VertexBufferView> vertexView;
		std::optional<IndexBufferView> indexView;

		TSize size = 0;
		TSize alignment = 0;

	};

}
