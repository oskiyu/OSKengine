#pragma once

#include "OSKmacros.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Un mesh representa una serie de primitivas (triángulos) de un
	/// modelo 3D que comparten unas mismas propiedades de material.
	/// </summary>
	/// 
	/// @note Los buffers con vértices e índices son propiedad del modelo 3D.
	/// 
	/// @note Un mesh únicamente almacena los índices que le corresponden,
	/// representados por el ID del primer índice y el número de índices.
	class OSKAPI_CALL Mesh3D {

	public:

		/// <summary>
		/// Crea el mesh que será compuesto por los los triángulos
		/// compuestos con los índices dados.
		/// </summary>
		/// 
		/// @pre El número de índices debe ser múltipo de 3.
		/// @pre El número de índices debe ser mayor que 0.
		Mesh3D(TSize numIndices, TSize firstIndex);


		/// <summary>
		/// Devuelve el número de índices que componen la malla de
		/// este mesh.
		/// </summary>
		/// 
		/// @note Mútliplo de 3.
		TSize GetNumberOfIndices() const;

		/// <summary>
		/// Offset del primer índice respecto al buffer de índices
		/// del modelo 3D al que pertenece este mesh.
		/// </summary>
		TSize GetFirstIndexId() const;

	private:

		TSize numIndices = 0;
		TSize firstIndex = 0;

	};

}
