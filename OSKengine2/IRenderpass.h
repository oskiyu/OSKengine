#pragma once

#include "OSKmacros.h"

namespace OSK {

	class GpuImage;
	enum class RenderpassType;

	class OSKAPI_CALL IRenderpass {

	public:

		virtual ~IRenderpass() = default;

		GpuImage* GetImage(TSize index) const;
		TSize GetNumberOfImages() const;

		virtual void SetImage(GpuImage* image) = 0;
		virtual void SetImages(GpuImage* image0, GpuImage* image1, GpuImage* image2) = 0;

		RenderpassType GetType() const;

		template <typename T> T* As() const requires std::is_base_of_v<IRenderpass, T> {
			return (T*)this;
		}

	protected:

		IRenderpass(RenderpassType mode);

		RenderpassType type;
		GpuImage* images[3];

	};

}
