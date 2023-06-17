#pragma once

#include "OSKmacros.h"
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

		
		/// @brief Devuelve el canal al que se va a acceder a trav�s
		/// de este view.
		/// Puede incluir varios canales al mismo tiempo.
		/// @return Canal(es) de la imagen accedidos.
		/// 
		/// @deprecated Acceder a trav�s de GetConfig().
		SampledChannel GetChannel() const;

		/// <summary>
		/// Permite saber si este view consiste en una �nica capa
		/// o referencia varias capas en un array.
		/// </summary>
		/// 
		/// @deprecated Acceder a trav�s de GetConfig().
		SampledArrayType GetArrayType() const;

		/// <summary>
		/// Devuelve el �ndice de la primera capa del array
		/// (o de la �nica capa si GetArrayType() es
		/// SampledArrayType::SINGLE_LAYER.
		/// </summary>
		/// 
		/// @deprecated Acceder a trav�s de GetConfig().
		UIndex32 GetBaseArrayLevel() const;

		/// <summary>
		/// N�mero de capas del array.
		/// </summary>
		/// 
		/// @note Si GetArrayType() es
		/// SampledArrayType::SINGLE_LAYER,
		/// devolver� 1.
		/// 
		/// @deprecated Acceder a trav�s de GetConfig().
		USize32 GetLayerCount() const;

		/// <summary>
		/// Contexto en el que se va a usar la imagen.
		/// </summary>
		/// 
		/// @deprecated Acceder a trav�s de GetConfig().
		ViewUsage GetViewUsage() const;

		/// @return Mip level m�s detallado.
		/// 
		/// @deprecated Acceder a trav�s de GetConfig().
		inline UIndex32 GetBaseMipLevel() const { return config.baseMipLevel; }

		/// @return Mip level menos detallado.
		/// 
		/// @deprecated Acceder a trav�s de GetConfig().
		inline UIndex32 GetTopMipLevel() const { return config.topMipLevel; }

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
		Vector3ui size = 0;

		const GpuImage* originalImage;

	};

}
