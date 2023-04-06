#pragma once

#include "OSKmacros.h"
#include "OwnedPtr.h"
#include "UniquePtr.hpp"
#include "Vector2.hpp"
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

		GpuImage(
			const Vector3ui& size, 
			GpuImageDimension dimension, 
			GpuImageUsage usage, 
			TSize numLayers, 
			Format format, 
			TSize numSamples, 
			GpuImageSamplerDesc samplerDesc);

		virtual ~GpuImage() override;

		OSK_DEFINE_AS(GpuImage);

		void _SetPhysicalSize(const Vector3ui& size);

		void SetBlock(OwnedPtr<IGpuMemoryBlock> buffer);

		IGpuMemoryBlock* GetMemory() const;
		IGpuMemorySubblock* GetBuffer() const;

		/// @return Resolución de la imagen, en píxeles.
		/// Para imágenes 2D, el componente Z es 1.
		/// Para imágenes 1D, los componentes Y y Z son 1.
		Vector3ui GetSize3D() const;

		/// @return Resolución de la imagen, en píxeles.
		/// Para imágenes 1D, el componente Y es 1.
		Vector2ui GetSize2D() const;

		/// @return Resolución de la imagen, en píxeles.
		TSize GetSize1D() const;

		Vector3ui GetPhysicalSize() const;

		/// @return Formato de la imagen.
		Format GetFormat() const;

		/// @return Dimensionalidad de la imagen.
		GpuImageDimension GetDimension() const;

		/// @return Uso que se le va a dar a la imagen.
		GpuImageUsage GetUsage() const;

		/// @return Número de capas de array.
		/// Para una imagen simple, 1.
		/// @note Será siempre >= 1.
		TSize GetNumLayers() const;

		/// @brief Devuelve la resolución del mip-level indicado.
		/// @param mipLevel Mip-level.
		/// @return Resolución, en píxeles.
		/// 
		/// @pre El mip level indicado debe existir en la imagen, 
		/// de lo contrario devolverá un valor incorrecto.
		/// @pre La imagen debe ser 2D o cubemap.
		Vector2ui GetMipLevelSize2D(TIndex mipLevel) const;

		/// @return Sampler por defecto de la imagen.
		GpuImageSamplerDesc GetImageSampler() const;

		/// <summary>
		/// Devuelve el número máximo de miplevels de esta imagen.
		/// </summary>
		unsigned int GetMipLevels() const;

		/// <summary>
		/// Devuelve el número máximo de miplevels de una imagen.
		/// </summary>
		/// <param name="sizeX">Ancho de la imagen.</param>
		/// <param name="sizeY">Alto de la imagen.</param>
		static TSize GetMipLevels(uint32_t sizeX, uint32_t sizeY);

		/// <summary>
		/// Devuelve el número de bytes que ocupa esta imagen en la memoria de la GPU.
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
		/// @warning Función puramente informativa: no cambia el layout real de la imagen. Para ello, debe usarse 
		/// ICommandList::TransitionImageLayout.
		void SetLayout(GpuImageLayout layout);

		/// <summary>
		/// Devuelve el layout actual de la imagen.
		/// </summary>
		/// 
		/// @warning Al ser una variable informativa (no vinculante), puede no devolver el layout real actual.
		/// 
		/// @warning Únicamente mostrará el valor real cuando se ejecute la lista de comandos con la cual
		/// se ha realizado la transición de layout.
		GpuImageLayout GetLayout() const;

		/// <summary>
		/// Devuelve el número de muestras que esta imagen puede tener en un renderizado con MSAA.
		/// </summary>
		TSize GetNumSamples() const;

		/// @brief Obtiene un image view con las características dadas.
		/// @param channel Canal de la imagen accedido por el view.
		/// @param arrayType Configura si es una única imagen o si el view afecta a todo un array.
		/// @param baseArrayLevel Capa base representada.
		/// @param layerCount Número de capas representadas (si @p arrayType es SampledArrayType::ARRAY).
		/// @param baseMipLevel Mip level más detallado.
		/// @param topMipLevel Mip level menos detallado.
		/// @param usage Uso del view.
		/// @return View con las características dadas.
		/// 
		/// @pre Si @p layerCount > 1, entonces @p arrayType debe ser SampledArrayType::ARRAY.
		/// @pre @p topMipLevel >= @p baseMipLevel.
		/// @pre Los mip levels indicados por el rango @p topMipLevel y @p baseMipLevel deben existir en la imagen original.
		IGpuImageView* GetView(const GpuImageViewConfig& viewConfig) const;

	protected:

		virtual OwnedPtr<IGpuImageView> CreateView(const GpuImageViewConfig& viewConfig) const = 0;

	private:

		mutable HashMap<GpuImageViewConfig, UniquePtr<IGpuImageView>> views;

		IGpuMemoryBlock* block = nullptr;
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
