#pragma once

#include "ApiCall.h"
#include "DefineAs.h"
#include "NumericTypes.h"

#include "IGpuObject.h"
#include "GpuBufferRange.h"
#include "IGpuImageSampler.h"

#include <string>
#include <string_view>

namespace OSK::ASSETS {
	class Texture;
}

namespace OSK::GRAPHICS {

	class GpuImage;
	class MaterialLayout;
	class ITopLevelAccelerationStructure;
	class GpuBuffer;
	class IGpuImageView;


	/// @brief Un slot contiene referencias a los recursos (UNIFORM BUFFER, TEXTURE, etc...) 
	/// a los que se accederá en los shaders.
	/// 
	/// @warning Establecer los recursos mediante Set<...> no actualizará los recursos que realmente se enviarán a los shaders,
	/// debe llamarse explícitamente a FlushUpdate().
	class OSKAPI_CALL IMaterialSlot : public IGpuObject {

	protected:

		IMaterialSlot(const MaterialLayout* layout, const std::string& name);

	public:

		virtual ~IMaterialSlot() = default;

		OSK_DEFINE_AS(IMaterialSlot);

		
		/// @brief Establece la imagen que será asignada al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// @param binding Nombre del binding al que se asignará la imagen.
		/// @param image Imagen que se asignará.
		/// @param arrayIndex Índice de la imagen, para arrays de imágenes.
		/// @param sampler Sampler que se usará para procesar la imagen.
		/// 
		/// @pre Si el binding no se corresponde con un array de imágenes,
		/// @p arrayIndex debe ser 0.
		/// 
		/// @note No surgirá efecto hasta que se llame a `FlushUpdate()`.
		virtual void SetGpuImage(
			std::string_view binding,
			const IGpuImageView& image,
			const IGpuImageSampler& sampler,
			UIndex32 arrayIndex = 0) = 0;

		/// @brief Establece la imagen que será asignada al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// @param binding Nombre del binding al que se asignará la imagen.
		/// @param image Imagen que se asignará.
		/// @param arrayIndex Índice de la imagen, para arrays de imágenes.
		/// @param samplerInfo Información del sampler que se usará para procesar la imagen.
		/// 
		/// @pre Si el binding no se corresponde con un array de imágenes,
		/// @p arrayIndex debe ser 0.
		/// 
		/// @note No surgirá efecto hasta que se llame a `FlushUpdate()`.
		void SetGpuImage(
			std::string_view binding,
			const IGpuImageView& image,
			const GpuImageSamplerDesc& samplerInfo,
			UIndex32 arrayIndex = 0);

		/// @brief Establece la imagen que será usada como storage image asignada al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// @param binding Nombre del binding al que se asignará la imagen.
		/// @param image Imagen que se asignará.
		/// @param arrayIndex Índice de la imagen, para arrays de imágenes.
		/// 
		/// @pre Si el binding no se corresponde con un array de imágenes,
		/// @p arrayIndex debe ser 0.
		/// 
		/// @note No surgirá efecto hasta que se llame a `FlushUpdate()`.
		virtual void SetStorageImage(
			std::string_view binding,
			const IGpuImageView& image,
			UIndex32 arrayIndex = 0) = 0;


		/// @brief Establece el UNIFORM BUFFER que será asignado al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// @param binding Nombre del binding al que se asignará el UNIFORM BUFFER.
		/// @param buffer Buffer asignado.
		/// @param range Rango del buffer que será visible. Por defecto, todo el buffer.
		/// @param arrayIndex Índice dentro del array.
		/// 
		/// @pre Si el binding no se corresponde con un array de buffers,
		/// @p arrayIndex debe ser 0.
		/// 
		/// @note No surgirá efecto hasta que se llame a `FlushUpdate()`.
		virtual void SetUniformBuffer(
			std::string_view binding,
			const GpuBuffer& buffer,
			const GpuBufferRange& range = GpuBufferRange::CreateFullRange(),
			UIndex32 arrayIndex = 0) = 0;

		/// @brief Establece el buffer que será usado como storage buffer asignado al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// @param binding Nombre del binding al que se asignará el STORAGE BUFFER.
		/// @param buffer Buffer asignado.
		/// @param range Rango del buffer que será visible. Por defecto, todo el buffer.
		/// @param arrayIndex Índice dentro del array.
		/// 
		/// @pre Si el binding no se corresponde con un array de buffers,
		/// @p arrayIndex debe ser 0.
		/// 
		/// @note No surgirá efecto hasta que se llame a `FlushUpdate()`.
		virtual void SetStorageBuffer(
			std::string_view binding,
			const GpuBuffer& buffer,
			const GpuBufferRange& range = GpuBufferRange::CreateFullRange(),
			UIndex32 arrayIndex = 0) = 0;


		/// @brief Establece la estructura de aceleración para trazado de rayos que será asignado al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// @param binding Nombre del binding al que se asignará la estructura de aceleración.
		/// @param accelerationStructure Estructura de aceleración a establecer.
		/// @param arrayIndex Índice dentro del array.
		/// 
		/// @pre Si el binding no se corresponde con un array de estructuas de aceleración,
		/// @p arrayIndex debe ser 0.
		/// 
		/// @note No surgirá efecto hasta que se llame a `FlushUpdate()`.
		virtual void SetAccelerationStructure(
			std::string_view binding,
			const ITopLevelAccelerationStructure& accelerationStructure,
			UIndex32 arrayIndex = 0) = 0;


		/// @brief Actualiza los recursos que se enviarán a los shaders.
		virtual void FlushUpdate() = 0;

		/// @return Nombre del slot.
		std::string_view GetName() const;
		
	protected:

		const MaterialLayout* GetLayout() const;

	private:

		const MaterialLayout* m_layout = nullptr;
		std::string m_name;

	};

}
