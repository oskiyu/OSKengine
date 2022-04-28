#pragma once

#include "IGpuImage.h"
#include "OglTypes.h"

namespace OSK::GRAPHICS {

	/// @deprecated OpenGL no implementado.
	class OSKAPI_CALL GpuImageOgl : public GpuImage {

	public:

		GpuImageOgl(const Vector3ui& size, GpuImageDimension dimension, GpuImageUsage usage, TSize numLayers, Format format);
		~GpuImageOgl();

		OglImageHandler GetHandler() const;
		void SetHandler(OglImageHandler handler);

	private:

		OglImageHandler handler = 0;

	};

}
