#pragma once

#include "OSKmacros.h"
#include <string>

namespace OSK::ASSETS {
	class Texture;
}

namespace OSK::GRAPHICS {

	class IGpuUniformBuffer;
	class GpuImage;
	class MaterialLayout;
	class GpuDataBuffer;
	class ITopLevelAccelerationStructure;

	/// <summary>
	/// Un slot contiene referencias a los recursos (UNIFORM BUFFER, TEXTURE, etc...) que se envían a la GPU.
	/// 
	/// @warning Establecer los recursos mediante Set<...> no actualizará los recursos que realmente se enviarán a los shaders,
	/// debe llamarse explícitamente a FlushUpdate().
	/// </summary>
	class OSKAPI_CALL IMaterialSlot {

	public:

		virtual ~IMaterialSlot() = default;

		template <typename T> T* As() const requires std::is_base_of_v<IMaterialSlot, T> {
			return (T*)this;
		}

		/// <summary>
		/// Establece el UNIFORM BUFFER que será asignado al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// 
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignará el UNIFORM BUFFER.</param>
		virtual void SetUniformBuffer(const std::string& binding, const IGpuUniformBuffer* buffer) = 0;

		/// <summary>
		/// Establece la textura que será asignada al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// 
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignará la textura.</param>
		void SetTexture(const std::string& binding, const ASSETS::Texture* texture);

		/// <summary>
		/// Establece la imagen que será asignada al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// 
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignará la imagen.</param>
		virtual void SetGpuImage(const std::string& binding, const GpuImage* image) = 0;

		/// <summary>
		/// Establece el buffer que será usado como storage buffer asignado al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// 
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		/// </summary>
		virtual void SetStorageBuffer(const std::string& binding, const GpuDataBuffer* buffer) = 0;

		/// <summary>
		/// Establece la imagen que será usada como storage image asignada al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// 
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		/// </summary>
		virtual void SetStorageImage(const std::string& binding, const GpuImage* image) = 0;

		/// <summary>
		/// Establece la estructura de aceleración para trazado de rayos que será asignado al binding con el nombre dado.
		/// Puede usarse esta función para alternar el recurso que está asignado al binding.
		/// 
		/// @warning No actualizará el recurso que realmente se usará en el shader, se debe llamar a FlushUpdate().
		/// </summary>
		virtual void SetAccelerationStructure(const std::string& binding, const ITopLevelAccelerationStructure* accelerationStructure) = 0;

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
