#pragma once

#include "ApiCall.h"
#include "OwnedPtr.h"

#include "VertexBufferView.h"
#include "IndexBufferView.h"

#include "GpuBarrierInfo.h"

#include <optional>


namespace OSK::GRAPHICS {

	class IGpuMemoryBlock;
	class IGpuMemorySubblock;
	class ICommandQueue;
	struct GpuBufferRange;

	/// @brief Representa un buffer que contiene informaci�n
	/// accesible desde la GPU.
	class OSKAPI_CALL GpuBuffer final {

	public:

		/// @brief Establece los par�metros del buffer.
		/// @param buffer Subbloque de memoria que tiene este buffer.
		/// @param size Tama�o del buffer. En bytes.
		/// @param alignment Alineamiento del contenido del buffer.
		/// @param ownerQueue Cola que posee el recurso.
		/// 
		/// @note No crea el buffer, el buffer es creado por IGpuMemoryAllocator.
		GpuBuffer(
			OwnedPtr<IGpuMemorySubblock> buffer, 
			USize64 size, 
			USize64 alignment,
			const ICommandQueue* ownerQueue);

		~GpuBuffer();

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

		/// @brief Mapea toda la regi�n de memoria del buffer, para poder escribirla
		/// desde la CPU.
		/// 
		/// @pre Debe haber sido creado con GpuSharedMemoryType::GPU_AND_CPU.
		/// @pre No debe haber ninguna regi�n de memoria del buffer previamente mapeada.
		void MapMemory();

		/// @brief Mapea la regi�n de memoria dada,para poder escribirla
		/// desde la CPU
		/// 
		/// @param size Tama�o de la regi�n, en bytes.
		/// @param offset Offset desde el inicio del buffer, en bytes.
		/// 
		/// @pre Debe haber sido creado con GpuSharedMemoryType::GPU_AND_CPU.
		/// @pre No debe haber ninguna regi�n de memoria del buffer previamente mapeada.
		/// @pre @p size + @p offset < GetSize().
		void MapMemory(USize64 size, USize64 offset);

		/// @brief Escribe la informaci�n en el buffer, a partir de la posici�n actual
		/// del cursor. Tambi�n hace avanzar el cursor.
		/// 
		/// @param data Informaci�n a escribir.
		/// @param size Tama�o de la informaci�n a escribir.
		/// 
		/// @pre La memoria debe estar mapeada.
		/// @pre @p size <= GetSize() - cursor.
		void Write(const void* data, USize64 size);

		/// @brief Escribe la informaci�n del elemento dado en el buffer, usando
		/// el cursor. Tambi�n hace avanzar el cursor.
		/// @tparam T Tipo de la estructura a escribir.
		/// @param data Informaci�n a escribir.
		/// 
		/// @pre La memoria debe estar mapeada.
		/// @pre @p size <= GetSize() - cursor.
		template <typename T>
		void Write(const T& data) {
			Write(&data, sizeof(T));
		}

		/// @brief Escribe la informaci�n en el buffer, sin usar el cursor.
		/// 
		/// @param data Informaci�n a escribir.
		/// @param size N�mero de bytes a escribir.
		/// @param offset Offset (en bytes) respecto al inicio del buffer.
		/// 
		/// @pre La memoria debe estar mapeada.
		/// @pre La informaci�n a escribir debe estar contenida dentro
		/// de la zona de memoria mapeada.
		void WriteOffset(const void* data, USize64 size, USize64 offset);

		/// @brief Desmapea la memoria.
		/// @pre La memoria debe estar mapeada.
		void Unmap();

		
		/// @brief Establece la posici�n del cursor.
		/// @param position Posici�n, en bytes, respecto al inicio del buffer.
		/// @pre La posici�n debe estar dentro de los l�mites del buffer.
		void SetCursor(UIndex64 position);

		/// @brief Establece la posici�n del cursor a 0.
		void ResetCursor();


		/// @return Devuelve el tama�o del buffer, en bytes.
		USize64 GetSize() const;

		/// @return Devuelve la alineaci�n del buffer, en bytes.
		USize64 GetAlignment() const;


		GpuBufferRange GetWholeBufferRange() const;

		IGpuMemorySubblock* GetMemorySubblock() const;
		IGpuMemoryBlock* GetMemoryBlock() const;

#pragma region Barriers

		/// @brief Establece el barrier actualmente aplicado, para facilitar la sincronizaci�n
		/// en futuras llamadas.
		/// @param barrier Barrier aplicado.
		void _UpdateCurrentBarrier(const GpuBarrierInfo& barrier);

		/// @return Barrier actualmente aplicado.
		const GpuBarrierInfo& GetCurrentBarrier() const;

#pragma endregion

		/// @return Cola de comandos que posee el recurso.
		/// @stablepointer
		const ICommandQueue* GetOwnerQueue() const;

		/// @brief Actualiza la variable de la clase que representa
		/// la cola de comandos que la posee.
		/// @param ownerQueue Nueva cola que posee el buffer.
		/// @pre @p ownerQueue debe ser estable.
		void _UpdateOwnerQueue(const ICommandQueue* ownerQueue);

#pragma region Queues

#pragma endregion


	protected:

		IGpuMemorySubblock* buffer = nullptr;

	private:

		/// @brief Cola de comandos que posee este recurso.
		const ICommandQueue* m_ownerQueue = nullptr;

		GpuBarrierInfo m_currentBarrier{};

		std::optional<VertexBufferView> vertexView;
		std::optional<IndexBufferView> indexView;

		USize64 size = 0;
		USize64 alignment = 0;

		std::string m_name = "UNNAMED";

	};

}
