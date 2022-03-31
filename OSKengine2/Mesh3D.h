#pragma once

#include "OSKmacros.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Un mesh representa una serie de primitivas (tri�ngulos) de un
	/// modelo 3D que comparten unas mismas propiedades de material.
	/// 
	/// Los buffers con v�rtices e �ndices son propiedad del modelo 3D,
	/// la clase mesh �nicamente almacena los �ndices que le corresponden,
	/// representados por el ID del primer �ndice y el n�mero de �ndices.
	/// </summary>
	class OSKAPI_CALL Mesh3D {

	public:

		Mesh3D(TSize numIndices, TSize firstIndex);

		TSize GetNumberOfIndices() const;
		TSize GetFirstIndexId() const;

	private:

		TSize numIndices = 0;
		TSize firstIndex = 0;

	};

}
