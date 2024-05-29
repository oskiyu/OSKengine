#pragma once

#include "ApiCall.h"
#include "DefineAs.h"
#include "ResourcesInFlight.h"

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


	/// @brief Un slot contiene referencias a los recursos (UNIFORM BUFFER, TEXTURE, etc...) que se env�an a la GPU.
	/// 
	/// @warning Establecer los recursos mediante Set<...> no actualizar� los recursos que realmente se enviar�n a los shaders,
	/// debe llamarse expl�citamente a FlushUpdate().
	class OSKAPI_CALL IMaterialSlot : public IGpuObject {

	public:

		virtual ~IMaterialSlot() = default;

		OSK_DEFINE_AS(IMaterialSlot);

		
		/// @brief Establece el UNIFORM BUFFER que ser� asignado al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// @param binding Nombre del binding al que se asignar� el UNIFORM BUFFER.
		/// @param buffer Buffer asignado.
		/// 
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		void SetUniformBuffer(
			const std::string& binding, 
			const GpuBuffer& buffer,
			UIndex32 arrayIndex = 0);

		/// @brief Establece el UNIFORM BUFFER que ser� asignado al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// @param binding Nombre del binding al que se asignar� el UNIFORM BUFFER.
		/// @param buffers Uniform buffers.
		/// 
		/// @note Habr� un uniform buffer por cada frame in flight.
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		virtual void SetUniformBuffers(
			const std::string& binding, 
			std::span<const GpuBuffer*, MAX_RESOURCES_IN_FLIGHT> buffers,
			UIndex32 arrayIndex = 0) = 0;


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
			SampledChannel channel = SampledChannel::COLOR,
			UIndex32 arrayIndex = 0);

		/// <summary>
		/// Establece las texturas que ser�n asignadas al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignar� la textura.</param>
		/// <param name="texture">Texturas.</param>
		/// 
		/// @note Habr� una textura por cada frame in flight.
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		
		/// @brief Establece las texturas que ser�n asignadas al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// @param binding 
		/// @param textures 
		/// @param channel 
		void SetTextures(
			const std::string& binding, 
			std::span<const ASSETS::Texture*, MAX_RESOURCES_IN_FLIGHT> textures,
			SampledChannel channel = SampledChannel::COLOR,
			UIndex32 arrayIndex = 0);

		/// <summary>
		/// Establece la imagen que ser� asignada al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignar� la imagen.</param>
		/// 
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		void SetGpuImage(
			const std::string& binding, 
			const IGpuImageView* image,
			UIndex32 arrayIndex = 0);

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
			std::span<const IGpuImageView*, MAX_RESOURCES_IN_FLIGHT> images,
			UIndex32 arrayIndex = 0) = 0;


		/// <summary>
		/// Establece el buffer que ser� usado como storage buffer asignado al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// </summary>
		/// 
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		void SetStorageBuffer(
			const std::string& binding, 
			const GpuBuffer* buffer,
			UIndex32 arrayIndex = 0);

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
			std::span<const GpuBuffer*, MAX_RESOURCES_IN_FLIGHT> buffers,
			UIndex32 arrayIndex = 0) = 0;


		/// <summary>
		/// Establece la imagen que ser� usada como storage image asignada al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// </summary>
		/// 
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		void SetStorageImage(
			const std::string& binding, 
			const IGpuImageView* image,
			UIndex32 arrayIndex = 0);

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
			std::span<const IGpuImageView*, MAX_RESOURCES_IN_FLIGHT> images,
			UIndex32 arrayIndex = 0) = 0;


		/// <summary>
		/// Establece la estructura de aceleraci�n para trazado de rayos que ser� asignado al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// </summary>
		/// 
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		void SetAccelerationStructure(
			const std::string& binding, 
			const ITopLevelAccelerationStructure* accelerationStructure,
			UIndex32 arrayIndex = 0);

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
			std::span<const ITopLevelAccelerationStructure*, MAX_RESOURCES_IN_FLIGHT> accelerationStructure,
			UIndex32 arrayIndex = 0) = 0;


		/// <summary>
		/// Actualiza los recursos que se enviar�n a los shaders.
		/// </summary>
		virtual void FlushUpdate() = 0;

		std::string_view GetName() const;
		
	protected:

		IMaterialSlot(const MaterialLayout* layout, const std::string& name);

		const MaterialLayout* m_layout = nullptr;
		std::string m_name;

	};

}
