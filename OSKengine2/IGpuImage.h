#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"
#include "UniquePtr.hpp"
#include "Vector2.hpp"

namespace OSK {

	class IGpuMemoryBlock;
	class IGpuMemorySubblock;
	enum class Format;

	class OSKAPI_CALL GpuImage {

	public:

		GpuImage(unsigned int sizeX, unsigned int sizeY, Format format);
		virtual ~GpuImage();

		void SetData(const void* data, TSize size);
		void SetBlock(OwnedPtr<IGpuMemoryBlock> buffer);

		IGpuMemoryBlock* GetMemory() const;
		IGpuMemorySubblock* GetBuffer() const;

		Vector2ui GetSize() const;
		Format GetFormat() const;

		unsigned int GetMipLevels() const;

		template <typename T> T* As() const requires std::is_base_of_v<GpuImage, T> {
			return (T*)this;
		}

		static TSize GetMipLevels(uint32_t sizeX, uint32_t sizeY);

	private:

		UniquePtr<IGpuMemoryBlock> block;
		IGpuMemorySubblock* buffer = nullptr;

		Vector2ui size = 0;
		unsigned int mipLevels = 0;
		Format format;

	};

}
