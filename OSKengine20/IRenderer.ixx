#include <string>

export module OSKengine.Graphics.IRenderer;

export import OSKengine.Util.Version;
export import OSKengine.WindowAPI;
export import OSKengine.RenderApiType;

import OSKengine.Memory.UniquePtr;
import OSKengine.Graphics.IGpu;

export namespace OSK {

	/// <summary>
	/// Proporciona la interfaz de un renderizador.
	/// Esta interfaz no implementa ningún código, se debe usar una instancia de
	/// RendererVulkan o RendererDx12.
	/// </summary>
	class IRenderer {

	public:

		/// <summary>
		/// Destruye el renderizador.
		/// </summary>
		virtual ~IRenderer() = default;

		/// <summary>
		/// Inicializa todo el sistema de renderizado.
		/// </summary>
		/// <param name="appName">Nombre de la aplicación / juego.</param>
		/// <param name="version">Versión de la aplicación / juego.</param>
		/// <param name="window">Ventana enlazada.</param>
		virtual void Initialize(const std::string& appName, const Version& version, const Window& window) = 0;

		/// <summary>
		/// Cierra el renderizador.
		/// También se llama en el destructor.
		/// </summary>
		virtual void Close() = 0;

		/// <summary>
		/// Castea el renderizador al tipo dado.
		/// Este tipo debe ser una implementación de esta interfaz.
		/// </summary>
		template <typename T> T* As() {
			return (T*)this;
		}

	protected:

		IRenderer() { }

		/// <summary>
		/// 
		/// </summary>
		UniquePtr<IGpu> currentGpu;

	private:

	};

}
