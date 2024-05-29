#pragma once

// Para IGame.
#include "Game.h"

// Para TDeltaTime deltaTime.
#include "NumericTypes.h"

namespace OSK {

	class EditorShowCommand;
	class EditorHideCommand;

	namespace UI {
		class Console;
	}


	/// @brief Clase base para un juego, que incluye la funcionalidad
	/// necesaria de debug y desarrollo:
	/// - Consola (insertada en la ínterfaz raíz, pero invisible).
	/// - Editor 3D (insertada en la ínterfaz raíz, pero invisible).
	class IDebugGame : public IGame {

	public:

		void RegisterConsoleCommands() override;
		void OnCreate() override;
		void OnTick_BeforeEcs(TDeltaTime deltaTime) override;

	protected:

		/// @return True si la consola debug está
		/// abierta.
		bool IsInConsole() const;

	private:

		EditorShowCommand* m_editorShowCommand = nullptr;
		EditorHideCommand* m_editorHideCommand = nullptr;

		UI::Console* m_console = nullptr;

	};

}
