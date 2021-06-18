#pragma once

#include "IToken.h"
#include "Scanner.h"

namespace OSK::SceneSystem::Loader {

	/// <summary>
	/// Token para cargar un terrain.
	/// </summary>
	class TerrainToken :public IToken {

	public:

		/// <summary>
		/// Token vac�o.
		/// </summary>
		TerrainToken();

		/// <summary>
		/// Procesa la informaci�n del token.
		/// </summary>
		void ProcessData(Scanner* sc) override;

		/// <summary>
		/// Ruta del terrain.
		/// </summary>
		std::string GetPath() const;

	private:

		/// <summary>
		/// Ruta del terrain.
		/// </summary>
		std::string path;

	};

}
