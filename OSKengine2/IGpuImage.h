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
	/// Representaci�n interna de una imagen en la GPU.
	/// 
	/// Puede referirse a la imagend e una textura, im�genes del swapchain, etc...
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

		/// @return Resoluci�n de la imagen, en p�xeles.
		/// Para im�genes 2D, el componente Z es 1.
		/// Para im�genes 1D, los componentes Y y Z son 1.
		Vector3ui GetSize3D() const;

		/// @return Resoluci�n de la imagen, en p�xeles.
		/// Para im�genes 1D, el componente Y es 1.
		Vector2ui GetSize2D() const;

		/// @return Resoluci�n de la imagen, en p�xeles.
		TSize GetSize1D() const;

		Vector3ui GetPhysicalSize() const;

		/// @return Formato de la imagen.
		Format GetFormat() const;

		/// @return Dimensionalidad de la imagen.
		GpuImageDimension GetDimension() const;

		/// @return Uso que se le va a dar a la imagen.
		GpuImageUsage GetUsage() const;

		/// @return N�mero de capas de array.
		/// Para una imagen simple, 1.
		/// @note Ser� siempre >= 1.
		TSize GetNumLayers() const;

		/// @brief Devuelve la resoluci�n del mip-level indicado.
		/// @param mipLevel Mip-level.
		/// @return Resoluci�n, en p�xeles.
		/// 
		/// @pre El mip level indicado debe existir en la imagen, 
		/// de lo contrario devolver� un valor incorrecto.
		/// @pre La imagen debe ser 2D o cubemap.
		Vector2ui GetMipLevelSize2D(TIndex mipLevel) const;

		/// @return Sampler por defecto de la imagen.
		GpuImageSamplerDesc GetImageSampler() const;

		/// <summary>
		/// Devuelve el n�mero m�ximo de miplevels de esta imagen.
		/// </summary>
		unsigned int GetMipLevels() const;

		/// <summary>
		/// Devuelve el n�mero m�ximo de miplevels de una imagen.
		/// </summary>
		/// <param name="sizeX">Ancho de la imagen.</param>
		/// <param name="sizeY">Alto de la imagen.</param>
		static TSize GetMipLevels(uint32_t sizeX, uint32_t sizeY);

		/// <summary>
		/// Devuelve el n�mero de bytes que ocupa esta imagen en la memoria de la GPU.
		/// </summary>
		/// <returns>Espacio ocupado en la GPU.</returns>
		/// 
		/// @note Debido a cuestiones de alineamiento y dem�s, puede ocupar m�s
		/// espacio del esperado.
		TSize GetNumberOfBytes() const;

		/// <summary>
		/// Devuelve el n�mero de bytes reales que ocupa esta imagen en la memoria de la GPU
		/// </summary>
		/// <returns>Espacio ocupado en la GPU.</returns>
		TSize GetPhysicalNumberOfBytes() const;

		/// <summary>
		/// Actualiza el layout de la imagen, para que represente el layout actual.
		/// </summary>
		/// @warning Funci�n interna: no llamar.
		/// 
		/// @warning Funci�n puramente informativa: no cambia el layout real de la imagen. Para ello, debe usarse 
		/// ICommandList::TransitionImageLayout.
		void SetLayout(GpuImageLayout layout);

		/// <summary>
		/// Devuelve el layout actual de la imagen.
		/// </summary>
		/// 
		/// @warning Al ser una variable informativa (no vinculante), puede no devolver el layout real actual.
		/// 
		/// @warning �nicamente mostrar� el valor real cuando se ejecute la lista de comandos con la cual
		/// se ha realizado la transici�n de layout.
		GpuImageLayout GetLayout() const;

		/// <summary>
		/// Devuelve el n�mero de muestras que esta imagen puede tener en un renderizado con MSAA.
		/// </summary>
		TSize GetNumSamples() const;

		/// @brief Obtiene un image view con las caracter�sticas dadas.
		/// @param channel Canal de la imagen accedido por el view.
		/// @param arrayType Configura si es una �nica imagen o si el view afecta a todo un array.
		/// @param baseArrayLevel Capa base representada.
		/// @param layerCount N�mero de capas representadas (si @p arrayType es SampledArrayType::ARRAY).
		/// @param baseMipLevel Mip level m�s detallado.
		/// @param topMipLevel Mip level menos detallado.
		/// @param usage Uso del view.
		/// @return View con las caracter�sticas dadas.
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
