#pragma once

#include <type_traits>

namespace OSK::GRAPHICS {

	class IGpuVertexDescription {

	public:

		virtual ~IGpuVertexDescription() = default;

		template <typename T> T* As() const requires std::is_base_of_v<IGpuVertexDescription, T> {
			return (T*)this;
		}

	};

}
