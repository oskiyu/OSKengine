#pragma once

#include "OSKmacros.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Un mesh representa una serie de primitivas (triángulos) de un
	/// modelo 3D que comparten unas mismas propiedades de material.
	/// 
	/// Los buffers con vértices e índices son propiedad del modelo 3D,
	/// la clase mesh únicamente almacena los índices que le corresponden,
	/// representados por el ID del primer índice y el número de índices.
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
