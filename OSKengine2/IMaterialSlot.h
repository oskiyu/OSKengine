#pragma once

#include "OSKmacros.h"
#include <string>

namespace OSK {

	class IGpuUniformBuffer;
	class GpuImage;
	class Texture;

	class MaterialLayout;

	/// <summary>
	/// Un slot contiene referencias a los recursos (UNIFORM BUFFER, TEXTURE, etc...) que se env�an a la GPU.
	/// Establecer los recursos mediante Set<...> no actualizar� los recursos que realmente se enviar�n a los shaders,
	/// debe llamarse expl�citamente a FlushUpdate().
	/// </summary>
	class OSKAPI_CALL IMaterialSlot {

	public:

		template <typename T> T* As() const requires std::is_base_of_v<IMaterialSlot, T> {
			return (T*)this;
		}

		/// <summary>
		/// Establece el UNIFORM BUFFER que ser� asignado al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// 
		/// No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignar� el UNIFORM BUFFER.</param>
		virtual void SetUniformBuffer(const std::string& binding, const IGpuUniformBuffer* buffer) = 0;

		/// <summary>
		/// Establece la textura que ser� asignada al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// 
		/// No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignar� la textura.</param>
		void SetTexture(const std::string& binding, const Texture* texture);

		/// <summary>
		/// Establece la imagen que ser� asignada al binding con el nombre dado.
		/// Puede usarse esta funci�n para alternar el recurso que est� asignado al binding.
		/// 
		/// No actualizar� el recurso que realmente se usar� en el shader, se debe llamar a FlushUpdate().
		/// </summary>
		/// <param name="binding">Nombre del binding al que se asignar� la imagen.</param>
		virtual void SetGpuImage(const std::string& binding, const GpuImage* image) = 0;

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
