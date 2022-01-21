#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"
#include "UniquePtr.hpp"
#include "Vector2.hpp"

namespace OSK {

	class IGpuMemoryBlock;
	class IGpuMemorySubblock;
	enum class Format;
	enum class GpuImageLayout;

	/// <summary>
	/// Representación interna de una imagen en la GPU.
	/// 
	/// Puede referirse a la imagend e una textura, imágenes del swapchain, etc...
	/// </summary>
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

		/// <summary>
		/// Devuelve el número máximo de miplevels de esta imagen.
		/// </summary>
		unsigned int GetMipLevels() const;

		template <typename T> T* As() const requires std::is_base_of_v<GpuImage, T> {
			return (T*)this;
		}

		/// <summary>
		/// Devuelve el número máximo de miplevels de una imagen.
		/// </summary>
		/// <param name="sizeX">Ancho de la imagen.</param>
		/// <param name="sizeY">Alto de la imagen.</param>
		static TSize GetMipLevels(uint32_t sizeX, uint32_t sizeY);

		void SetLayout(GpuImageLayout layout);
		GpuImageLayout GetLayout() const;

	private:

		UniquePtr<IGpuMemoryBlock> block;
		IGpuMemorySubblock* buffer = nullptr;

		Vector2ui size = 0;
		unsigned int mipLevels = 0;
		Format format;
		GpuImageLayout currentLayout;

	};

}
