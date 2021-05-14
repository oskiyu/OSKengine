#pragma once

#include "IToken.h"
#include "Scanner.h"

namespace OSK::SceneSystem::Loader {

	/// <summary>
	/// Token con la versión de la especificación del script de la escena.
	/// </summary>
	class VersionToken : public IToken {

	public:

		/// <summary>
		/// Token vacío.
		/// </summary>
		VersionToken();

		/// <summary>
		/// Procesa la información del token.
		/// </summary>
		void ProcessData(Scanner* sc) override;

		/// <summary>
		/// La versión del spec del script.
		/// </summary>
		int GetVersion() const;

	private:

		/// <summary>
		/// La versión del spec del script.
		/// </summary>
		int version = 0;

	};

}