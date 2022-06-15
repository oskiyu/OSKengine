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
	/// Un slot contiene referencias a los recursos (UNIFORM BUFFER, TEXTURE, etc...) que se env�an a la GPU.
	/// 
	/// @warning Establecer los recursos mediante Set<...> no actualizar� los recursos que realmente se enviar�n a los shaders,
	/// debe llamarse expl�citamente a FlushUpdate().
	/// </summary>
	class OSKAPI_CALL IMaterialSlot {

	public:

		virtual ~IMaterialSlot() = default;

		template <typename T> T* As() const requires std::is_base_of_v<IMaterialSlot, T> {
			return (T*)this;
		}

		/// <summary>
		/// Establece el UNIFORM BUFFER que ser� asignado al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// 
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignar� el UNIFORM BUFFER.</param>
		virtual void SetUniformBuffer(const std::string& binding, const IGpuUniformBuffer* buffer) = 0;

		/// <summary>
		/// Establece la textura que ser� asignada al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// 
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignar� la textura.</param>
		void SetTexture(const std::string& binding, const ASSETS::Texture* texture);

		/// <summary>
		/// Establece la imagen que ser� asignada al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// 
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignar� la imagen.</param>
		virtual void SetGpuImage(const std::string& binding, const GpuImage* image) = 0;

		/// <summary>
		/// Establece el buffer que ser� usado como storage buffer asignado al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// 
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		/// </summary>
		virtual void SetStorageBuffer(const std::string& binding, const GpuDataBuffer* buffer) = 0;

		/// <summary>
		/// Establece la imagen que ser� usada como storage image asignada al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// 
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		/// </summary>
		virtual void SetStorageImage(const std::string& binding, const GpuImage* image) = 0;

		/// <summary>
		/// Establece la estructura de aceleraci�n para trazado de rayos que ser� asignado al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// 
		/// @warning No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		/// </summary>
		virtual void SetAccelerationStructure(const std::string& binding, const ITopLevelAccelerationStructure* accelerationStructure) = 0;

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
