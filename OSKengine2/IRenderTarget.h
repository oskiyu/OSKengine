#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"

namespace OSK {

	class GpuImage;
	class ISwapcchain;

	class OSKAPI_CALL IRenderTarget {

	public:

		void SetImage(GpuImage* image);
		GpuImage* GetImage() const;

	private:

		UniquePtr<GpuImage> image;
		ISwapcchain* swapchain;

	};

}
