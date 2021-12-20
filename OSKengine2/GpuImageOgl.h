#pragma once

#include "IGpuImage.h"
#include "OglTypes.h"

namespace OSK {

	class OSKAPI_CALL GpuImageOgl : public GpuImage {

	public:

		GpuImageOgl(unsigned int sizeX, unsigned int sizeY, Format format);
		~GpuImageOgl();

		OglImageHandler GetHandler() const;
		void SetHandler(OglImageHandler handler);

	private:

		OglImageHandler handler = 0;

	};

}
