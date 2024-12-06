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
	/// a los que se acceder� en los shaders.
	/// 
	/// @warning Establecer los recursos mediante Set<...> no actualizar� los recursos que realmente se enviar�n a los shaders,
	/// debe llamarse expl�citamente a FlushUpdate().
	class OSKAPI_CALL IMaterialSlot : public IGpuObject {

	protected:

		IMaterialSlot(const MaterialLayout* layout, const std::string& name);

	public:

		virtual ~IMaterialSlot() = default;

		OSK_DEFINE_AS(IMaterialSlot);

		
		/// @brief Establece la imagen que ser� asignada al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// @param binding Nombre del binding al que se asignar� la imagen.
		/// @param image Imagen que se asignar�.
		/// @param arrayIndex �ndice de la imagen, para arrays de im�genes.
		/// @param sampler Sampler que se usar� para procesar la imagen.
		/// 
		/// @pre Si el binding no se corresponde con un array de im�genes,
		/// @p arrayIndex debe ser 0.
		/// 
		/// @note No surgir� efecto hasta que se llame a `FlushUpdate()`.
		virtual void SetGpuImage(
			std::string_view binding,
			const IGpuImageView& image,
			const IGpuImageSampler& sampler,
			UIndex32 arrayIndex = 0) = 0;

		/// @brief Establece la imagen que ser� asignada al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// @param binding Nombre del binding al que se asignar� la imagen.
		/// @param image Imagen que se asignar�.
		/// @param arrayIndex �ndice de la imagen, para arrays de im�genes.
		/// @param samplerInfo Informaci�n del sampler que se usar� para procesar la imagen.
		/// 
		/// @pre Si el binding no se corresponde con un array de im�genes,
		/// @p arrayIndex debe ser 0.
		/// 
		/// @note No surgir� efecto hasta que se llame a `FlushUpdate()`.
		void SetGpuImage(
			std::string_view binding,
			const IGpuImageView& image,
			const GpuImageSamplerDesc& samplerInfo,
			UIndex32 arrayIndex = 0);

		/// @brief Establece la imagen que ser� usada como storage image asignada al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// @param binding Nombre del binding al que se asignar� la imagen.
		/// @param image Imagen que se asignar�.
		/// @param arrayIndex �ndice de la imagen, para arrays de im�genes.
		/// 
		/// @pre Si el binding no se corresponde con un array de im�genes,
		/// @p arrayIndex debe ser 0.
		/// 
		/// @note No surgir� efecto hasta que se llame a `FlushUpdate()`.
		virtual void SetStorageImage(
			std::string_view binding,
			const IGpuImageView& image,
			UIndex32 arrayIndex = 0) = 0;


		/// @brief Establece el UNIFORM BUFFER que ser� asignado al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// @param binding Nombre del binding al que se asignar� el UNIFORM BUFFER.
		/// @param buffer Buffer asignado.
		/// @param range Rango del buffer que ser� visible. Por defecto, todo el buffer.
		/// @param arrayIndex �ndice dentro del array.
		/// 
		/// @pre Si el binding no se corresponde con un array de buffers,
		/// @p arrayIndex debe ser 0.
		/// 
		/// @note No surgir� efecto hasta que se llame a `FlushUpdate()`.
		virtual void SetUniformBuffer(
			std::string_view binding,
			const GpuBuffer& buffer,
			const GpuBufferRange& range = GpuBufferRange::CreateFullRange(),
			UIndex32 arrayIndex = 0) = 0;

		/// @brief Establece el buffer que ser� usado como storage buffer asignado al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// @param binding Nombre del binding al que se asignar� el STORAGE BUFFER.
		/// @param buffer Buffer asignado.
		/// @param range Rango del buffer que ser� visible. Por defecto, todo el buffer.
		/// @param arrayIndex �ndice dentro del array.
		/// 
		/// @pre Si el binding no se corresponde con un array de buffers,
		/// @p arrayIndex debe ser 0.
		/// 
		/// @note No surgir� efecto hasta que se llame a `FlushUpdate()`.
		virtual void SetStorageBuffer(
			std::string_view binding,
			const GpuBuffer& buffer,
			const GpuBufferRange& range = GpuBufferRange::CreateFullRange(),
			UIndex32 arrayIndex = 0) = 0;


		/// @brief Establece la estructura de aceleraci�n para trazado de rayos que ser� asignado al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// @param binding Nombre del binding al que se asignar� la estructura de aceleraci�n.
		/// @param accelerationStructure Estructura de aceleraci�n a establecer.
		/// @param arrayIndex �ndice dentro del array.
		/// 
		/// @pre Si el binding no se corresponde con un array de estructuas de aceleraci�n,
		/// @p arrayIndex debe ser 0.
		/// 
		/// @note No surgir� efecto hasta que se llame a `FlushUpdate()`.
		virtual void SetAccelerationStructure(
			std::string_view binding,
			const ITopLevelAccelerationStructure& accelerationStructure,
			UIndex32 arrayIndex = 0) = 0;


		/// @brief Actualiza los recursos que se enviar�n a los shaders.
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
