#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"

namespace OSK {

	class IGpuMemorySubblock;

	class OSKAPI_CALL GpuDataBuffer {

	public:

		GpuDataBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);
		virtual ~GpuDataBuffer();

		void MapMemory();
		void MapMemory(TSize size, TSize offset);
		void Write(const void* data, TSize size);
		void WriteOffset(const void* data, TSize size, TSize offset);
		void Unmap();

		TSize GetSize() const;
		TSize GetAlignment() const;

	protected:

		void Free();

		OwnedPtr<IGpuMemorySubblock> buffer;

	private:

		TSize size = 0;
		TSize alignment = 0;

	};

}