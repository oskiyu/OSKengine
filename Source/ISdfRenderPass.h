#pragma once

#include "ApiCall.h"
#include "UniquePtr.hpp"
#include "OSKengine.h"

#include "RenderPass.h"
#include "ISdfRenderer2D.h"
#include "SdfBindlessRenderer2D.h"

namespace OSK::GRAPHICS {

	/// @brief Pase de renderizado que usa un renderizador
	/// SDF 2D.
	/// 
	/// Tratará de elegir el modelo de renderizador SDF 2D
	/// más óptimo de acuerdo con las características
	/// de la GPU.
	/// 
	/// @todo Implementar renderizador SDF 2D que no sea
	/// bindless.
	class OSKAPI_CALL ISdfRenderPass : public IRenderPass {

	protected:

		ISdfRenderPass();

		/// @return Renderizador SDF 2D.
		///
		/// Cuando se obtiene dentro de la función
		/// de renderizado, habrá que comenzarlo (`Begin()`)
		/// y cerrarlo (`End()`) manualmente.
		ISdfRenderer2D* GetSdfRenderer();

	private:

		UniquePtr<ISdfRenderer2D> m_sdfRenderer;

	};

}
