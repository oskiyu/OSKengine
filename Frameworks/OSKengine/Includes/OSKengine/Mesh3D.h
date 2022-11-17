#pragma once

#include "OSKmacros.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Un mesh representa una serie de primitivas (tri�ngulos) de un
	/// modelo 3D que comparten unas mismas propiedades de material.
	/// </summary>
	/// 
	/// @note Los buffers con v�rtices e �ndices son propiedad del modelo 3D.
	/// 
	/// @note Un mesh �nicamente almacena los �ndices que le corresponden,
	/// representados por el ID del primer �ndice y el n�mero de �ndices.
	class OSKAPI_CALL Mesh3D {

	public:

		/// <summary>
		/// Crea el mesh que ser� compuesto por los los tri�ngulos
		/// compuestos con los �ndices dados.
		/// </summary>
		/// 
		/// @pre El n�mero de �ndices debe ser m�ltipo de 3.
		/// @pre El n�mero de �ndices debe ser mayor que 0.
		Mesh3D(TSize numIndices, TSize firstIndex);


		/// <summary>
		/// Devuelve el n�mero de �ndices que componen la malla de
		/// este mesh.
		/// </summary>
		/// 
		/// @note M�tliplo de 3.
		TSize GetNumberOfIndices() const;

		/// <summary>
		/// Offset del primer �ndice respecto al buffer de �ndices
		/// del modelo 3D al que pertenece este mesh.
		/// </summary>
		TSize GetFirstIndexId() const;

	private:

		TSize numIndices = 0;
		TSize firstIndex = 0;

	};

}
