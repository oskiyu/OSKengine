#pragma once

#include "GpuImageSamplerDesc.h"
#include "GpuImageUsage.h"
#include "Format.h"

namespace OSK::GRAPHICS {
	
	/// <summary>
	/// 
	/// </summary>
	struct RenderTargetAttachmentInfo {

		/// <summary> Formato de la imagen. </summary>
		Format format = Format::UNKNOWN;

		/// <summary>
		/// Uso de la imagen.
		/// Dependiendo del render target, tendrá un conjunto
		/// de usos por defecto.
		/// </summary>
		GpuImageUsage usage = GpuImageUsage::UNKNOWN;

		/// <summary> Sampler por defecto. </summary>
		GpuImageSamplerDesc sampler = GpuImageSamplerDesc::CreateDefault();

		std::string name = "";

	};
	
}
