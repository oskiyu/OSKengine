#pragma once

#include "IToken.h"
#include "Scanner.h"

namespace OSK::SceneSystem::Loader {

	/// <summary>
	/// Token para cargar un skybox.
	/// </summary>
	class SkyboxToken : public IToken {

	public:

		/// <summary>
		/// Token vac�o.
		/// </summary>
		SkyboxToken();

		/// <summary>
		/// Procesa la informaci�n del token.
		/// </summary>
		void ProcessData(Scanner* sc) override;

		/// <summary>
		/// Ruta del skybox.
		/// </summary>
		std::string GetPath() const;

	private:

		/// <summary>
		/// Ruta del skybox.
		/// </summary>
		std::string path;

	};

}
