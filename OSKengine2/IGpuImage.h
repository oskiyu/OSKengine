#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"
#include "UniquePtr.hpp"
#include "Vector3.hpp"
#include "GpuImageSamplerDesc.h"

#include "IGpuMemoryBlock.h"
#include "IGpuObject.h"

#include "IGpuImageView.h"
#include "IGpuMemorySubblock.h"
#include "DynamicArray.hpp"
#include "HashMap.hpp"

namespace OSK::GRAPHICS {

	enum class Format;
	class IGpuMemorySubblock;
	enum class GpuImageLayout;
	enum class GpuImageDimension;
	enum class GpuImageUsage;
	class IGpu;

	/// <summary>
	/// Representación interna de una imagen en la GPU.
	/// 
	/// Puede referirse a la imagend e una textura, imágenes del swapchain, etc...
	/// </summary>
	class OSKAPI_CALL GpuImage : public IGpuObject {

	public:

		GpuImage(const Vector3ui& size, GpuImageDimension dimension, GpuImageUsage usage, TSize numLayers, Format format, TSize numSamples, GpuImageSamplerDesc samplerDesc);
		virtual ~GpuImage();

		void _SetPhysicalSize(const Vector3ui& size);

		void SetData(const void* data, TSize size);
		void SetBlock(OwnedPtr<IGpuMemoryBlock> buffer);

		IGpuMemoryBlock* GetMemory() const;
		IGpuMemorySubblock* GetBuffer() const;

		Vector3ui GetSize() const;
		Vector3ui GetPhysicalSize() const;
		Format GetFormat() const;
		GpuImageDimension GetDimension() const;
		GpuImageUsage GetUsage() const;
		TSize GetNumLayers() const;

		GpuImageSamplerDesc GetImageSampler() const;

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

		/// <summary>
		/// Devuelve el número de bytes que ocupa esta imagen en la memoria de la GPU
		/// </summary>
		/// <returns>Espacio ocupado en la GPU.</returns>
		/// 
		/// @note Debido a cuestiones de alineamiento y demás, puede ocupar más
		/// espacio del esperado.
		TSize GetNumberOfBytes() const;

		/// <summary>
		/// Devuelve el número de bytes reales que ocupa esta imagen en la memoria de la GPU
		/// </summary>
		/// <returns>Espacio ocupado en la GPU.</returns>
		TSize GetPhysicalNumberOfBytes() const;

		/// <summary>
		/// Actualiza el layout de la imagen, para que represente el layout actual.
		/// </summary>
		/// @warning Función interna: no llamar.
		/// 
		/// @note Función puramente informativa: no cambia el layout real de la imagen. Para ello, debe usarse 
		/// ICommandList::TransitionImageLayout.
		void SetLayout(GpuImageLayout layout);

		/// <summary>
		/// Devuelve el layout actual de la imagen.
		/// </summary>
		/// 
		/// @warning Al ser una variable informativa (no vinculante), puede no devolver el layout real actual.
		/// 
		/// @note Únicamente mostrará el valor real cuando se ejecute la lista de comandos con la cual
		/// se ha realizado la transición de layout.
		GpuImageLayout GetLayout() const;

		/// <summary>
		/// Devuelve el número de muestras que esta imagen puede tener en un renderizado con MSAA.
		/// </summary>
		TSize GetNumSamples() const;

		IGpuImageView* GetView(SampledChannel channel, SampledArrayType arrayType, TSize baseArrayLevel, TSize layerCount, ViewUsage usage) const;

	protected:

		virtual OwnedPtr<IGpuImageView> CreateView(SampledChannel channel, SampledArrayType arrayType, TSize baseArrayLevel, TSize layerCount, ViewUsage usage) const = 0;

	private:

		mutable HashMap<IGpuImageView, UniquePtr<IGpuImageView>> views;

		UniquePtr<IGpuMemoryBlock> block;
		IGpuMemorySubblock* buffer = nullptr;

		Vector3ui size = 0;
		Vector3ui physicalSize = 0;

		GpuImageSamplerDesc samplerDesc{};

		TSize mipLevels = 0;
		TSize numSamples = 0;
		Format format;
		GpuImageLayout currentLayout;
		GpuImageDimension dimension;
		GpuImageUsage usage;
		TSize numLayers = 0;

	};

}
