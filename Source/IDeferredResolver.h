#pragma once

#include "IShaderPass.h"

#include "UniquePtr.hpp"
#include "MaterialInstance.h"

namespace OSK::GRAPHICS {

	/// @brief Pase de renderizado que realiza la resoluci�n
	/// de un renderizado en diferido.
	class OSKAPI_CALL IDeferredResolver : public IShaderPass {

	protected:

		/// @param name Nombre del pase de resoluci�n.
		explicit IDeferredResolver(const std::string& name) : IShaderPass(name) {}

	public:

		virtual ~IDeferredResolver() = default;

	};

}
