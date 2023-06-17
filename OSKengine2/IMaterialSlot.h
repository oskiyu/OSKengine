#pragma once

#include "OSKmacros.h"
#include "GpuImageUsage.h"
#include <string>
#include "IGpuObject.h"

#include <span>

namespace OSK::ASSETS {
	class Texture;
}

namespace OSK::GRAPHICS {

	class GpuImage;
	class MaterialLayout;
	class ITopLevelAccelerationStructure;
	class GpuBuffer;
	class IGpuImageView;


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
		void SetUniformBuffer(
			const std::string& binding, 
			const GpuBuffer& buffer);

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
			std::span<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> buffers) = 0;


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
			std::span<const ASSETS::Texture*, NUM_RESOURCES_IN_FLIGHT> textures,
			SampledChannel channel = SampledChannel::COLOR);

		/// <summary>
		/// Establece la imagen que será asignada al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignará la imagen.</param>
		/// 
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		void SetGpuImage(
			const std::string& binding, 
			const IGpuImageView* image);

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
			std::span<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> images) = 0;


		/// <summary>
		/// Establece el buffer que será usado como storage buffer asignado al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// </summary>
		/// 
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		void SetStorageBuffer(
			const std::string& binding, 
			const GpuBuffer* buffer);

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
			std::span<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> buffers) = 0;


		/// <summary>
		/// Establece la imagen que será usada como storage image asignada al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// </summary>
		/// 
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		void SetStorageImage(
			const std::string& binding, 
			const IGpuImageView* image);

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
			std::span<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> images) = 0;


		/// <summary>
		/// Establece la estructura de aceleración para trazado de rayos que será asignado al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// </summary>
		/// 
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		void SetAccelerationStructure(
			const std::string& binding, 
			const ITopLevelAccelerationStructure* accelerationStructure);

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
			std::span<const ITopLevelAccelerationStructure*, NUM_RESOURCES_IN_FLIGHT> accelerationStructure) = 0;


		/// <summary>
		/// Actualiza los recursos que se enviarán a los shaders.
		/// </summary>
		virtual void FlushUpdate() = 0;

		std::string_view GetName() const;
		
	protected:

		IMaterialSlot(const MaterialLayout* layout, const std::string& name);

		const MaterialLayout* layout = nullptr;
		std::string name;

	};

}
