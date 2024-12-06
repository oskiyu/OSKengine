#pragma once

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

enum DXGI_FORMAT;

namespace OSK::GRAPHICS {

	enum class Format;

	/// <summary>
	/// Obtiene la representación nativa del formato para DirectX 12.
	/// </summary>
	DXGI_FORMAT GetFormatDx12(Format format);

}

#endif
