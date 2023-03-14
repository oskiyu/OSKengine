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
	/// Un slot contiene referencias a los recursos (UNIFORM BUFFER, TEXTURE, etc...) que se env�an a la GPU.
	/// 
	/// @warning Establecer los recursos mediante Set<...> no actualizar� los recursos que realmente se enviar�n a los shaders,
	/// debe llamarse expl�citamente a FlushUpdate().
	/// </summary>
	class OSKAPI_CALL IMaterialSlot : public IGpuObject {

	public:

		virtual ~IMaterialSlot() = default;

		OSK_DEFINE_AS(IMaterialSlot);

		/// <summary>
		/// Establece el UNIFORM BUFFER que ser� asignado al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// 
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignar� el UNIFORM BUFFER.</param>
		virtual void SetUniformBuffer(
			const std::string& binding, 
			const IGpuUniformBuffer* buffer) = 0;

		/// <summary>
		/// Establece el UNIFORM BUFFER que ser� asignado al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignar� el UNIFORM BUFFER.</param>
		/// <param name="buffers">Uniform buffers.</param>
		/// 
		/// @note Habr� un uniform buffer por cada frame in flight.
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		virtual void SetUniformBuffers(
			const std::string& binding, 
			const IGpuUniformBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]) = 0;


		/// <summary>
		/// Establece la textura que ser� asignada al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignar� la textura.</param>
		/// 
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		void SetTexture(
			const std::string& binding, 
			const ASSETS::Texture* texture, 
			SampledChannel channel = SampledChannel::COLOR);

		/// <summary>
		/// Establece las texturas que ser�n asignadas al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignar� la textura.</param>
		/// <param name="texture">Texturas.</param>
		/// 
		/// @note Habr� una textura por cada frame in flight.
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		void SetTextures(
			const std::string& binding, 
			const ASSETS::Texture* texture[NUM_RESOURCES_IN_FLIGHT], 
			SampledChannel channel = SampledChannel::COLOR);

		/// <summary>
		/// Establece la imagen que ser� asignada al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignar� la imagen.</param>
		/// 
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		virtual void SetGpuImage(
			const std::string& binding, 
			const GpuImage* image, 
			SampledChannel channel = SampledChannel::COLOR, 
			SampledArrayType arrayType = SampledArrayType::SINGLE_LAYER, 
			TSize arrayLevel = 0) = 0;

		/// <summary>
		/// Establece la imagen que ser� asignada al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignar� la imagen.</param>
		/// <param name="images">Im�genes.</param>
		/// 
		/// @note Habr� una imagen por cada frame in flight.
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		virtual void SetGpuImages(
			const std::string& binding, 
			const GpuImage* images[NUM_RESOURCES_IN_FLIGHT], 
			SampledChannel channel = SampledChannel::COLOR, 
			SampledArrayType arrayType = SampledArrayType::SINGLE_LAYER, 
			TSize arrayLevel = 0) = 0;


		/// <summary>
		/// Establece el buffer que ser� usado como storage buffer asignado al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// </summary>
		/// 
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		virtual void SetStorageBuffer(
			const std::string& binding, 
			const GpuDataBuffer* buffer) = 0;

		/// <summary>
		/// Establece el buffer que ser� usado como storage buffer asignado al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignar� el buffer.</param>
		/// <param name="buffers">Buffers.</param>
		/// 
		/// @note Habr� un buffer por cada frame in flight.
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		virtual void SetStorageBuffers(
			const std::string& binding, 
			const GpuDataBuffer* buffers[NUM_RESOURCES_IN_FLIGHT]) = 0;


		/// <summary>
		/// Establece la imagen que ser� usada como storage image asignada al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// </summary>
		/// 
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		virtual void SetStorageImage(
			const std::string& binding, 
			const GpuImage* image, 
			SampledArrayType arrayType = SampledArrayType::SINGLE_LAYER, 
			TSize arrayLayer = 0) = 0;

		/// <summary>
		/// Establece la imagen que ser� usada como storage image asignada al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignar� la imagen.</param>
		/// <param name="images">Im�genes.</param>
		/// 
		/// @note Habr� una imagen por cada frame in flight.
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		virtual void SetStorageImages(
			const std::string& binding, 
			const GpuImage* images[NUM_RESOURCES_IN_FLIGHT], 
			SampledArrayType arrayType = SampledArrayType::SINGLE_LAYER,
			TSize arrayLayer = 0) = 0;


		/// <summary>
		/// Establece la estructura de aceleraci�n para trazado de rayos que ser� asignado al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// </summary>
		/// 
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		virtual void SetAccelerationStructure(
			const std::string& binding, 
			const ITopLevelAccelerationStructure* accelerationStructure) = 0;

		/// <summary>
		/// Establece la estructura de aceleraci�n para trazado de rayos que ser� asignado al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignar� la imagen.</param>
		/// <param name="accelerationStructure">Estructuras de aceleraci�n.</param>
		/// 
		/// @note Habr� una estructura de aceleraci�n por cada frame in flight.
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		virtual void SetAccelerationStructures(
			const std::string& binding, 
			const ITopLevelAccelerationStructure* accelerationStructure[NUM_RESOURCES_IN_FLIGHT]) = 0;


		/// <summary>
		/// Actualiza los recursos que se enviar�n a los shaders.
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
