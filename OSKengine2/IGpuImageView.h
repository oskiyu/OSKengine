#pragma once

#include "OSKmacros.h"
#include "GpuImageUsage.h"
#include "GpuImageViewConfig.h"

#include "Vector3.hpp"
#include "Vector2.hpp"

namespace OSK::GRAPHICS {

	class GpuImage;
	

	/// @brief Un image view describe cómo y a qúe parte de una imagen
	/// se va a acceder al usarla.
	class OSKAPI_CALL IGpuImageView {

	public:

		virtual ~IGpuImageView() = default;

		/// @brief Crea un image view.
		/// @param originalImage Imagen a partir de la que se obtuvo el view.
		/// @param config Configuración del view.
		/// 
		/// @pre Si @p config.arrayLayerCount > 1, entonces @p arrayType debe ser SampledArrayType::ARRAY
		/// @pre @p config.arrayLayerCount != 0.
		IGpuImageView(
			const GpuImage& originalImage,
			const GpuImageViewConfig& config);

		OSK_DEFINE_AS(IGpuImageView);

		
		/// @brief Devuelve el canal al que se va a acceder a través
		/// de este view.
		/// Puede incluir varios canales al mismo tiempo.
		/// @return Canal(es) de la imagen accedidos.
		/// 
		/// @deprecated Acceder a través de GetConfig().
		SampledChannel GetChannel() const;

		/// <summary>
		/// Permite saber si este view consiste en una única capa
		/// o referencia varias capas en un array.
		/// </summary>
		/// 
		/// @deprecated Acceder a través de GetConfig().
		SampledArrayType GetArrayType() const;

		/// <summary>
		/// Devuelve el índice de la primera capa del array
		/// (o de la única capa si GetArrayType() es
		/// SampledArrayType::SINGLE_LAYER.
		/// </summary>
		/// 
		/// @deprecated Acceder a través de GetConfig().
		UIndex32 GetBaseArrayLevel() const;

		/// <summary>
		/// Número de capas del array.
		/// </summary>
		/// 
		/// @note Si GetArrayType() es
		/// SampledArrayType::SINGLE_LAYER,
		/// devolverá 1.
		/// 
		/// @deprecated Acceder a través de GetConfig().
		USize32 GetLayerCount() const;

		/// <summary>
		/// Contexto en el que se va a usar la imagen.
		/// </summary>
		/// 
		/// @deprecated Acceder a través de GetConfig().
		ViewUsage GetViewUsage() const;

		/// @return Mip level más detallado.
		/// 
		/// @deprecated Acceder a través de GetConfig().
		inline UIndex32 GetBaseMipLevel() const { return config.baseMipLevel; }

		/// @return Mip level menos detallado.
		/// 
		/// @deprecated Acceder a través de GetConfig().
		inline UIndex32 GetTopMipLevel() const { return config.topMipLevel; }

		/// @return Configuración completa del view.
		inline const GpuImageViewConfig& GetConfig() const { return config; }

		/// @brief Devuelve la imagen a partir de la que se creó el view.
		/// @return Imagen accedida por el view.
		/// 
		/// @pre La imagen a partir de la que se creó el view debe tener estabilidad de puntero.
		inline const GpuImage& GetImage() const { return *originalImage;  }


		/// @return Tamaño de la imagen, en 3 dimensiones.
		/// @see GpuImage.
		inline Vector3ui GetSize3D() const { return size; }

		/// @return Tamaño de la imagen, en 2 dimensiones.
		/// @see GpuImage.
		inline Vector2ui GetSize2D() const { return { size.x, size.y }; }

		/// @return Tamaño de la imagen, en 1 dimensión.
		/// @see GpuImage.
		inline USize32 GetSize1D() const { return size.x; };

	private:

		GpuImageViewConfig config{};
		Vector3ui size = 0;

		const GpuImage* originalImage;

	};

}
