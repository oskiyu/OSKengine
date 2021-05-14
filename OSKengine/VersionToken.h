#pragma once

#include "IToken.h"
#include "Scanner.h"

namespace OSK::SceneSystem::Loader {

	/// <summary>
	/// Token con la versi�n de la especificaci�n del script de la escena.
	/// </summary>
	class VersionToken : public IToken {

	public:

		/// <summary>
		/// Token vac�o.
		/// </summary>
		VersionToken();

		/// <summary>
		/// Procesa la informaci�n del token.
		/// </summary>
		void ProcessData(Scanner* sc) override;

		/// <summary>
		/// La versi�n del spec del script.
		/// </summary>
		int GetVersion() const;

	private:

		/// <summary>
		/// La versi�n del spec del script.
		/// </summary>
		int version = 0;

	};

}