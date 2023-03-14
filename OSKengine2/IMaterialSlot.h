#pragma once

#include "OSKmacros.h"
#include "GpuImageUsage.h"
#include <string>
#include "IGpuObject.h"

namespace OSK::ASSETS {
	class Texture;
}

namespace OSK::GRAPHICS {

	class IGpuUniformBuffer;
	class GpuImage;
	class MaterialLayout;
	class ITopLevelAccelerationStructure;
	class GpuDataBuffer;


	/// <summary>
	/// Un slot contiene referencias a los recursos (UNIFORM BUFFER, TEXTURE, etc...) que se envían a la GPU.
	/// 
	/// @warning Establecer los recursos mediante Set<...> no actualizará los recursos que realmente se enviarán a los shaders,
	/// debe llamarse explícitamente a FlushUpdate().
	/// </summary>
	class OSKAPI_CALL IMaterialSlot : public IGpuObject {

	public:

		virtual ~IMaterialSlot() = default;

		OSK_DEFINE_AS(IMaterialSlot);

		/// <summary>
		/// Establece el UNIFORM BUFFER que será asignado al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// 
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignará el UNIFORM BUFFER.</param>
		virtual void SetUniformBuffer(
			const std::string& binding, 
			const IGpuUniformBuffer* buffer) = 0;

		/// <summary>
		/// Establece el UNIFORM BUFFER que será asignado al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignará el UNIFORM BUFFER.</param>
		/// <param name="buffers">Uniform buffers.</param>
		/// 
		/// @note Habrá un uniform buffer por cada frame in flight.
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		virtual void SetUniformBuffers(
			const std::string& binding, 
			const IGpuUniformBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]) = 0;


		/// <summary>
		/// Establece la textura que será asignada al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignará la textura.</param>
		/// 
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		void SetTexture(
			const std::string& binding, 
			const ASSETS::Texture* texture, 
			SampledChannel channel = SampledChannel::COLOR);

		/// <summary>
		/// Establece las texturas que serán asignadas al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignará la textura.</param>
		/// <param name="texture">Texturas.</param>
		/// 
		/// @note Habrá una textura por cada frame in flight.
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		void SetTextures(
			const std::string& binding, 
			const ASSETS::Texture* texture[NUM_RESOURCES_IN_FLIGHT], 
			SampledChannel channel = SampledChannel::COLOR);

		/// <summary>
		/// Establece la imagen que será asignada al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignará la imagen.</param>
		/// 
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		virtual void SetGpuImage(
			const std::string& binding, 
			const GpuImage* image, 
			SampledChannel channel = SampledChannel::COLOR, 
			SampledArrayType arrayType = SampledArrayType::SINGLE_LAYER, 
			TSize arrayLevel = 0) = 0;

		/// <summary>
		/// Establece la imagen que será asignada al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignará la imagen.</param>
		/// <param name="images">Imágenes.</param>
		/// 
		/// @note Habrá una imagen por cada frame in flight.
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		virtual void SetGpuImages(
			const std::string& binding, 
			const GpuImage* images[NUM_RESOURCES_IN_FLIGHT], 
			SampledChannel channel = SampledChannel::COLOR, 
			SampledArrayType arrayType = SampledArrayType::SINGLE_LAYER, 
			TSize arrayLevel = 0) = 0;


		/// <summary>
		/// Establece el buffer que será usado como storage buffer asignado al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// </summary>
		/// 
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		virtual void SetStorageBuffer(
			const std::string& binding, 
			const GpuDataBuffer* buffer) = 0;

		/// <summary>
		/// Establece el buffer que será usado como storage buffer asignado al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignará el buffer.</param>
		/// <param name="buffers">Buffers.</param>
		/// 
		/// @note Habrá un buffer por cada frame in flight.
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		virtual void SetStorageBuffers(
			const std::string& binding, 
			const GpuDataBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]) = 0;


		/// <summary>
		/// Establece la imagen que será usada como storage image asignada al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// </summary>
		/// 
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		virtual void SetStorageImage(
			const std::string& binding, 
			const GpuImage* image, 
			SampledArrayType arrayType = SampledArrayType::SINGLE_LAYER, 
			TSize arrayLayer = 0) = 0;

		/// <summary>
		/// Establece la imagen que será usada como storage image asignada al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignará la imagen.</param>
		/// <param name="images">Imágenes.</param>
		/// 
		/// @note Habrá una imagen por cada frame in flight.
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		virtual void SetStorageImages(
			const std::string& binding, 
			const GpuImage* images[NUM_RESOURCES_IN_FLIGHT], 
			SampledArrayType arrayType = SampledArrayType::SINGLE_LAYER,
			TSize arrayLayer = 0) = 0;


		/// <summary>
		/// Establece la estructura de aceleración para trazado de rayos que será asignado al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// </summary>
		/// 
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		virtual void SetAccelerationStructure(
			const std::string& binding, 
			const ITopLevelAccelerationStructure* accelerationStructure) = 0;

		/// <summary>
		/// Establece la estructura de aceleración para trazado de rayos que será asignado al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignará la imagen.</param>
		/// <param name="accelerationStructure">Estructuras de aceleración.</param>
		/// 
		/// @note Habrá una estructura de aceleración por cada frame in flight.
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		virtual void SetAccelerationStructures(
			const std::string& binding, 
			const ITopLevelAccelerationStructure* accelerationStructure[NUM_RESOURCES_IN_FLIGHT]) = 0;


		/// <summary>
		/// Actualiza los recursos que se enviarán a los shaders.
		/// </summary>
		virtual void FlushUpdate() = 0;

		std::string GetName() const;
		
	protected:

		IMaterialSlot(const MaterialLayout* layout, const std::string& name);

		const MaterialLayout* layout = nullptr;
		std::string name;

	private:

	};

}
