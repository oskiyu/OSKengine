#pragma once

#include "ApiCall.h"
#include "UniquePtr.hpp"

#include "DynamicArray.hpp"
#include "HashMap.hpp"

#include "Vector2.hpp"
#include "Vector3.hpp"

#include "GpuImageCreateInfo.h"

#include "IGpuMemoryBlock.h"
#include "IGpuObject.h"

#include "IGpuImageView.h"
#include "IGpuMemorySubblock.h"

#include "GpuBarrierInfo.h"

namespace OSK::GRAPHICS {

	enum class Format;
	class IGpuMemorySubblock;
	enum class GpuImageLayout;
	enum class GpuImageDimension;
	enum class GpuImageUsage;
	class IGpu;
	class ICommandQueue;
	

	/// @brief Representación interna de una imagen en la GPU.
	class OSKAPI_CALL GpuImage : public IGpuObject {

	public:

		/// @brief Índice del array de imágenes.
		using ArrayLevelIndex = UIndex32;

		/// @brief Índice de niveles de mip.
		using MipLevelIndex = UIndex32;

	public:

		virtual ~GpuImage() override;

		OSK_DISABLE_COPY(GpuImage);
		OSK_DEFAULT_MOVE_OPERATOR(GpuImage);

		OSK_DEFINE_AS(GpuImage);
		

#pragma region Memory

		/// @return Bloque de memoria en el que la imagen
		/// está alojada.
		const IGpuMemoryBlock* GetMemory() const;
		IGpuMemoryBlock* GetMemory();

		/// @return Subbloque de memoria en el que la imagen
		/// está alojada.
		const IGpuMemorySubblock* GetBuffer() const;
		IGpuMemorySubblock* GetBuffer();

		void SetBlock(UniquePtr<IGpuMemoryBlock>&& buffer);

#pragma endregion

#pragma region Resoluciones

		/// @return Resolución de la imagen, en píxeles.
		/// Para imágenes 2D, el componente Z es 1.
		/// Para imágenes 1D, los componentes Y y Z son 1.
		Vector3ui GetSize3D() const;

		/// @return Resolución de la imagen, en píxeles.
		/// Para imágenes 1D, el componente Y es 1.
		Vector2ui GetSize2D() const;

		/// @return Resolución de la imagen, en píxeles.
		USize32 GetSize1D() const;

		void _SetPhysicalSize(const Vector3ui& size);
		Vector3ui GetPhysicalSize() const;

#pragma endregion

		


		/// @return Formato de la imagen.
		Format GetFormat() const;

		/// @return Dimensionalidad de la imagen.
		GpuImageDimension GetDimension() const;

		/// @return Uso que se le va a dar a la imagen.
		GpuImageUsage GetUsage() const;

		/// @return Número de capas de array.
		/// Para una imagen simple, 1.
		/// @note Será siempre >= 1.
		USize32 GetNumLayers() const;

		/// @brief Devuelve la resolución del mip-level indicado.
		/// @param mipLevel Mip-level.
		/// @return Resolución, en píxeles.
		/// 
		/// @pre El mip level indicado debe existir en la imagen, 
		/// de lo contrario devolverá un valor incorrecto.
		/// @pre La imagen debe ser 2D o cubemap.
		Vector2ui GetMipLevelSize2D(UIndex32 mipLevel) const;

		/// @return Sampler por defecto de la imagen.
		GpuImageSamplerDesc GetImageSampler() const;

		/// @return Número máximo de miplevels de esta imagen.
		USize32 GetMipLevels() const;

		/// @param sizeX Resolución en X.
		/// @param sizeY Resolución en Y.
		/// @return Número máximo de miplevels de una imagen.
		static USize32 GetMipLevels(uint32_t sizeX, uint32_t sizeY);


		/// @return Devuelve el número de bytes que ocupa esta imagen en la memoria de la GPU.
		USize64 GetNumberOfBytes() const;

		/// @return Devuelve el número de bytes real que ocupa esta imagen en la memoria de la GPU.
		/// @note Debido a cuestiones de alineamiento y demás, puede ocupar más
		/// espacio del esperado.
		USize64 GetPhysicalNumberOfBytes() const;

		/// @return Devuelve el número de muestras que esta imagen puede tener en un renderizado con MSAA.
		USize32 GetMsaaSampleCount() const;

		/// @return Tiling de la imagen.
		GpuImageTiling GetImageTiling() const;

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
		/// 
		/// @throws ImageViewCreationException Si hay algún error al crear el view.
		const IGpuImageView* GetView(const GpuImageViewConfig& viewConfig) const;


		/// @brief Establece el barrier actualmente aplicado, para facilitar la sincronización
		/// en futuras llamadas.
		/// @param barrier Barrier aplicado.
		void SetCurrentBarrier(const GpuBarrierInfo& barrier);

		/// @return Barrier actualmente aplicado.
		const GpuBarrierInfo& GetCurrentBarrier() const;


		/// @brief Establece información sobre el layout de la imagen.
		/// @param baseArrayLevel Nivel de array base.
		/// @param arrayLevelCount Número de niveles de array.
		/// @param baseMipLevel Nivel base de mip.
		/// @param mipLevelCount Número de niveles de mip.
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

#pragma region Queues

		/// @return Cola de comandos que posee el recurso.
		/// @stablepointer
		const ICommandQueue* GetOwnerQueue() const;

		/// @brief Actualiza la variable de la clase que representa
		/// la cola de comandos que la posee.
		/// @param ownerQueue Nueva cola que posee la imagen.
		/// @pre @p ownerQueue debe ser estable.
		void _UpdateOwnerQueue(const ICommandQueue* ownerQueue);

#pragma endregion


	protected:

		/// @pre @p ownerQueue debe ser estable.
		GpuImage(
			const GpuImageCreateInfo& info,
			const ICommandQueue* ownerQueue);

		/// @brief Crea un image view con la configuración indicada.
		/// @param viewConfig Configuración del view.
		/// @return View.
		/// @throws ImageViewCreationException Si hay algún error al crear el view.
		virtual UniquePtr<IGpuImageView> CreateView(const GpuImageViewConfig& viewConfig) const = 0;

	private:

		GpuBarrierInfo m_currentBarrier{};

		mutable std::unordered_map<GpuImageViewConfig, UniquePtr<IGpuImageView>> m_views;

		std::unordered_map<ArrayLevelIndex, std::unordered_map<MipLevelIndex, GpuImageLayout>> m_layouts;

		// Caso especial: bloque exclusivo para la imagen.
		UniquePtr<IGpuMemoryBlock> m_block;
		UniquePtr<IGpuMemorySubblock> m_buffer;

		Vector3ui m_physicalSize = Vector3ui::Zero;

		GpuImageCreateInfo m_imageInfo;

		USize32 m_mipLevels = 0;

		/// @brief Cola de comandos que posee este recurso.
		const ICommandQueue* m_ownerQueue = nullptr;

	};

}
