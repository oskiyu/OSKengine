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

#include "GpuBarrierInfo.h"

namespace OSK::GRAPHICS {

	enum class Format;
	class IGpuMemorySubblock;
	enum class GpuImageLayout;
	enum class GpuImageDimension;
	enum class GpuImageUsage;
	class IGpu;


	/// @brief Describe un rango de una imagen,
	/// incluyendo capas, niveles de mip y canales.
	struct GpuImageRange {

		/// @brief �ndice de la primera capa que ser� afectada por el barrier.
		/// @pre Si la imagen NO es array, debe ser 0.
		UIndex32 baseLayer = 0;

		/// @brief N�mero de capas del array afectadas por el barrier.
		USize32 numLayers = ALL_IMAGE_LAYERS;

		/// @brief Nivel m�s bajo de mip que ser� afectado por el barrier.
		UIndex32 baseMipLevel = 0;
		/// @brief N�mero de niveles de mip que ser�n afectados por el barrier.
		USize32 numMipLevels = ALL_MIP_LEVELS;


		/// @brief Canal(es) afectados por el barrier.
		SampledChannel channel = SampledChannel::COLOR;

	};


	/// <summary>
	/// Representaci�n interna de una imagen en la GPU.
	/// 
	/// Puede referirse a la imagend e una textura, im�genes del swapchain, etc...
	/// </summary>
	class OSKAPI_CALL GpuImage : public IGpuObject {

	public:

		using ArrayLevelIndex = UIndex32;
		using MipLevelIndex = UIndex32;

	public:

		GpuImage(
			const Vector3ui& size, 
			GpuImageDimension dimension, 
			GpuImageUsage usage, 
			USize32 numLayers, 
			Format format, 
			USize32 numSamples,
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
		USize32 GetSize1D() const;

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
		USize32 GetNumLayers() const;

		/// @brief Devuelve la resoluci�n del mip-level indicado.
		/// @param mipLevel Mip-level.
		/// @return Resoluci�n, en p�xeles.
		/// 
		/// @pre El mip level indicado debe existir en la imagen, 
		/// de lo contrario devolver� un valor incorrecto.
		/// @pre La imagen debe ser 2D o cubemap.
		Vector2ui GetMipLevelSize2D(UIndex32 mipLevel) const;

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
		static USize32 GetMipLevels(uint32_t sizeX, uint32_t sizeY);

		/// <summary>
		/// Devuelve el n�mero de bytes que ocupa esta imagen en la memoria de la GPU.
		/// </summary>
		/// <returns>Espacio ocupado en la GPU.</returns>
		/// 
		/// @note Debido a cuestiones de alineamiento y dem�s, puede ocupar m�s
		/// espacio del esperado.
		USize64 GetNumberOfBytes() const;

		/// <summary>
		/// Devuelve el n�mero de bytes reales que ocupa esta imagen en la memoria de la GPU
		/// </summary>
		/// <returns>Espacio ocupado en la GPU.</returns>
		USize64 GetPhysicalNumberOfBytes() const;

		/// <summary>
		/// Devuelve el n�mero de muestras que esta imagen puede tener en un renderizado con MSAA.
		/// </summary>
		USize32 GetNumSamples() const;

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
		/// 
		/// @throws ImageViewCreationException Si hay alg�n error al crear el view.
		IGpuImageView* GetView(const GpuImageViewConfig& viewConfig) const;


		/// @brief Establece el barrier actualmente aplicado, para facilitar la sincronizaci�n
		/// en futuras llamadas.
		/// @param barrier Barrier aplicado.
		void SetCurrentBarrier(const GpuBarrierInfo& barrier);

		/// @return Barrier actualmente aplicado.
		const GpuBarrierInfo& GetCurrentBarrier() const;


		/// @brief Establece informaci�n sobre el layout de la imagen.
		/// @param baseArrayLevel Nivel de array base.
		/// @param arrayLevelCount N�mero de niveles de array.
		/// @param baseMipLevel Nivel base de mip.
		/// @param mipLevelCount N�mero de niveles de mip.
		/// @param layout Layout del rango.
		void _SetLayout(
			ArrayLevelIndex baseArrayLevel,
			USize32 arrayLevelCount,
			MipLevelIndex baseMipLevel,
			USize32 mipLevelCount,
			GpuImageLayout layout);

		/// @brief Establece el layout de toda la imagen a UNDEFINED.
		void _InitDefaultLayout();

		/// @brief Obtiene el layout de la imagen (en tiempo de comando).
		/// @param arrayLevel Nivel de array.
		/// @param mipLevel Nivel de mip.
		/// @return Layout.
		GpuImageLayout _GetLayout(ArrayLevelIndex arrayLevel, MipLevelIndex mipLevel) const;

	protected:

		/// @brief Crea un image view con la configuraci�n indicada.
		/// @param viewConfig Configuraci�n del view.
		/// @return View.
		/// @throws ImageViewCreationException Si hay alg�n error al crear el view.
		virtual OwnedPtr<IGpuImageView> CreateView(const GpuImageViewConfig& viewConfig) const = 0;

	private:

		GpuBarrierInfo currentBarrier{};

		mutable HashMap<GpuImageViewConfig, UniquePtr<IGpuImageView>> views;

		HashMap<ArrayLevelIndex, HashMap<MipLevelIndex, GpuImageLayout>> layouts;

		IGpuMemoryBlock* block = nullptr;
		IGpuMemorySubblock* buffer = nullptr;

		Vector3ui size = 0;
		Vector3ui physicalSize = 0;

		GpuImageSamplerDesc samplerDesc{};

		USize32 mipLevels = 0;
		USize32 numSamples = 0;
		Format format;
		GpuImageLayout currentLayout;
		GpuImageDimension dimension;
		GpuImageUsage usage;
		USize32 numLayers = 0;

	};

}
