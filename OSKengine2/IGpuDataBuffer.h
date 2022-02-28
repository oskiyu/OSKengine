#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"

namespace OSK {

	class IGpuMemorySubblock;
	class IGpuMemoryBlock;

	/// <summary>
	/// Representa un buffer genérico en la GPU.
	/// </summary>
	class OSKAPI_CALL GpuDataBuffer {

	public:

		GpuDataBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);
		virtual ~GpuDataBuffer();

		void MapMemory();
		void MapMemory(TSize size, TSize offset);
		void Write(const void* data, TSize size);
		void WriteOffset(const void* data, TSize size, TSize offset);
		void Unmap();

		void SetCursor(TSize position);
		void ResetCursor();

		TSize GetSize() const;
		TSize GetAlignment() const;

		IGpuMemorySubblock* GetMemorySubblock() const;
		IGpuMemoryBlock* GetMemoryBlock() const;

	protected:

		void Free();

		OwnedPtr<IGpuMemorySubblock> buffer;

	private:

		TSize size = 0;
		TSize alignment = 0;

	};

}
