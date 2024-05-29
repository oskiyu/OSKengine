#pragma once

#include "ApiCall.h"
#include "NumericTypes.h"

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "GpuImageUsage.h"

namespace OSK::GRAPHICS {

	/// @brief Par�metros para la copia de una imagen a otra
	/// en la GPU.
	struct OSKAPI_CALL CopyImageInfo {

		static CopyImageInfo CreateDefault1D(USize32 size);
		static CopyImageInfo CreateDefault2D(Vector2ui size);
		static CopyImageInfo CreateDefault3D(Vector3ui size);

		void SetCopySize(USize32 size);
		void SetCopySize(Vector2ui size);
		void SetCopySize(Vector3ui size);

		void SetSourceOffset(USize32 offset);
		void SetSourceOffset(Vector2ui offset);
		void SetSourceOffset(Vector3ui offset);

		void SetDestinationOffset(USize32 offset);
		void SetDestinationOffset(Vector2ui offset);
		void SetDestinationOffset(Vector3ui offset);

		void SetCopyAllLevels();

		/// @brief Localizaci�n del �rea copiada de la imagen
		/// de origen. Esquina superior izquierda.
		Vector3ui sourceOffset = Vector3ui::Zero;

		/// @brief Localizaci�n del �rea copiada de la imagen
		/// final. Esquina superior izquierda.
		Vector3ui destinationOffset = Vector3ui::Zero;

		/// @brief �rea copiada.
		Vector3ui copySize = Vector3ui::Zero;


		/// @brief Capa de origen a partir de la cual se copiar�.
		UIndex32 sourceArrayLevel = 0;

		/// @brief Capa de destino a partir de la cual se escribir�.
		UIndex32 destinationArrayLevel = 0;

		/// @brief N�mero de capas que se van a copiar.
		/// Para copiar todas las capas, establecer como ALL_ARRAY_LEVELS.
		UIndex32 numArrayLevels = 1;


		/// @brief Mip level de la imagen de origen desde el cual se copiar�.
		UIndex32 sourceMipLevel = 0;

		/// @brief Mip level de la imagen de destino al que se copiar�.
		UIndex32 destinationMipLevel = 0;


		/// @brief Canal desde el cual se leer� la informaci�n.
		SampledChannel sourceChannel = SampledChannel::COLOR;
		/// @brief Canal sobre el que se realizar� la copia.
		SampledChannel destinationChannel = SampledChannel::COLOR;

		const static UIndex32 ALL_ARRAY_LEVELS = 0;

	};

}
