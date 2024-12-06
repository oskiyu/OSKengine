#pragma once

#include "ApiCall.h"
#include "DefineAs.h"

#include "GpuImageUsage.h"
#include "GpuImageViewConfig.h"

#include "Vector3.hpp"
#include "Vector2.hpp"

namespace OSK::GRAPHICS {

	class GpuImage;
	

	/// @brief Un image view describe c�mo y a q�e parte de una imagen
	/// se va a acceder al usarla.
	class OSKAPI_CALL IGpuImageView {

	public:

		virtual ~IGpuImageView() = default;

		/// @brief Crea un image view.
		/// @param originalImage Imagen a partir de la que se obtuvo el view.
		/// @param config Configuraci�n del view.
		/// 
		/// @pre Si @p config.arrayLayerCount > 1, entonces @p arrayType debe ser SampledArrayType::ARRAY
		/// @pre @p config.arrayLayerCount != 0.
		IGpuImageView(
			const GpuImage& originalImage,
			const GpuImageViewConfig& config);

		OSK_DEFINE_AS(IGpuImageView);

		
		/// @return Configuraci�n completa del view.
		inline const GpuImageViewConfig& GetConfig() const { return config; }

		/// @brief Devuelve la imagen a partir de la que se cre� el view.
		/// @return Imagen accedida por el view.
		/// 
		/// @pre La imagen a partir de la que se cre� el view debe tener estabilidad de puntero.
		inline const GpuImage& GetImage() const { return *originalImage;  }


		/// @return Tama�o de la imagen, en 3 dimensiones.
		/// @see GpuImage.
		inline Vector3ui GetSize3D() const { return size; }

		/// @return Tama�o de la imagen, en 2 dimensiones.
		/// @see GpuImage.
		inline Vector2ui GetSize2D() const { return { size.x, size.y }; }

		/// @return Tama�o de la imagen, en 1 dimensi�n.
		/// @see GpuImage.
		inline USize32 GetSize1D() const { return size.x; };

	private:

		GpuImageViewConfig config{};
		Vector3ui size = Vector3ui::Zero;

		const GpuImage* originalImage;

	};

}
