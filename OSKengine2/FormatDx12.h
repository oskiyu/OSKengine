#pragma once

enum DXGI_FORMAT;

namespace OSK {

	enum class Format;

	/// <summary>
	/// Obtiene la representaci�n nativa del formato para DirectX 12.
	/// </summary>
	DXGI_FORMAT GetFormatDx12(Format format);

}
