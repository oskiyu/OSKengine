#pragma once

namespace OSK::GRAPHICS {

	/// <summary>
	/// Define si un renderpass es un renderpass intermedio o el
	/// renderpass final que renderiza la imagen que se mostrará en
	/// el monitor.
	/// </summary>
	enum class RenderpassType {

		/// <summary>
		/// El renderpass es intermedio.
		/// </summary>
		INTERMEDIATE,

		/// <summary>
		/// El renderpass renderiza al swapchain.
		/// </summary>
		FINAL

	};

}
