#pragma once

#include "OSKmacros.h"

namespace OSK {

	class MaterialLayout;

	class OSKAPI_CALL IPipelineLayout {

	public:

		IPipelineLayout(const MaterialLayout* layout);
		virtual ~IPipelineLayout() = default;

		template <typename T> T* As() const requires std::is_base_of_v<IPipelineLayout, T> {
			return (T*)this;
		}

	};

}
