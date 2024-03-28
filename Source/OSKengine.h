#pragma once

#include "OSKmacros.h"

#include "UniquePtr.hpp"
#include "Version.h"
#include <string>

#include "Logger.h"
#include "IDisplay.h"
#include "IRenderer.h"
#include "AssetManager.h"
#include "EntityComponentSystem.h"
#include "IAudioApi.h"
#include "IUserInput.h"
#include "InputManager.h"
#include "Console.h"
#include "Uuid.h"


namespace OSK {
	
	namespace GRAPHICS {
		enum class RenderApiType;
	}


	/// @brief Clase principal de OSKengine.
	class OSKAPI_CALL Engine {

	public:

		Engine() = delete;
		Engine(const Engine&) = delete;
		Engine(Engine&&) = delete;

		Engine& operator=(const Engine& e) = delete;
		Engine& operator=(Engine&& e) = delete;

		static void Create(GRAPHICS::RenderApiType type);
		static void Close();

		static float GetCurrentTime();


		/// @return Puntero al logger del motor.
		/// @note El puntero es estable.
		/// @warning Será nullptr hasta que no se llame a Engine::Create.
		static IO::Logger* GetLogger();

		/// @return Puntero a la consola por defecto del motor.
		/// @note El puntero es estable.
		/// @warning Será nullptr hasta que no se llame a Engine::Create.
		static IO::Console* GetConsole();

		/// <summary> Devuelve un puntero al display del juego. </summary>
		/// @note El puntero es estable.
		/// @warning Será nullptr hasta que no se llame a Engine::Create.
		static IO::IDisplay* GetDisplay();

		/// <summary> Devuelve un puntero al renderizador del motor. </summary>
		/// @note El puntero es estable.
		/// @warning Será nullptr hasta que no se llame a Engine::Create.
		/// @warning El renderizador no será usable hasta que no se llame a IRenderer::Initialize.
		static GRAPHICS::IRenderer* GetRenderer();

		/// <summary> Devuelve un puntero al sistema de assets del motor. </summary>
		/// @note El puntero es estable.
		/// @warning Será nullptr hasta que no se llame a Engine::Create.
		static ASSETS::AssetManager* GetAssetManager();

		/// <summary> Devuelve un puntero al ECS del motor. </summary>
		/// @note El puntero es estable.
		/// @warning Será nullptr hasta que no se llame a Engine::Create.
		static ECS::EntityComponentSystem* GetEcs();

		/// @brief Devuelve un puntero a los estados del input del motor.
		/// @note El puntero es estable.
		/// @warning Será nullptr hasta que no se llame a Engine::Create.
		static IO::IUserInput* GetInput();

		/// <summary> Devuelve un puntero al sistema de input del motor. </summary>
		/// @note El puntero es estable.
		/// @warning Será nullptr hasta que no se llame a Engine::Create.
		static IO::InputManager* GetInputManager();

		/// @brief Devuelve un puntero al sistema de input del motor.
		/// @note El puntero es estable.
		/// @warning Será nullptr hasta que no se llame a Engine::Create.
		static AUDIO::IAudioApi* GetAudioApi();

		/// @brief Devuelve un puntero al sistema de asignación de UUIDs.
		/// @note El puntero es estable.
		/// @warning Será nullptr hasta que no se llame a Engine::Create.
		static UuidProvider* GetUuidProvider();


		/// @return Devuelve la versión del motor. 
		static Version GetVersion();
		/// @return Devuelve la build del motor, en formato aaaa.mm.dd.n
		static std::string_view GetBuild();

		static void RegisterBuiltinAssets();
		static void RegisterBuiltinComponents();
		static void RegisterBuiltinSystems();
		static void RegisterBuiltinEvents();
		static void RegisterBuiltinVertices();

		/// @return Índice del fotograma actual del juego,
		/// medido desde que se inició el juego en esta sesión.
		static UIndex64 GetCurrentGameFrameIndex();

		static void Update();

	private:

		static UniquePtr<IO::Logger> logger;
		static UniquePtr<IO::Console> console;
		static UniquePtr<IO::IDisplay> display;
		static UniquePtr<GRAPHICS::IRenderer> renderer;
		static UniquePtr<ASSETS::AssetManager> assetManager;
		static UniquePtr<ECS::EntityComponentSystem> entityComponentSystem;
		static UniquePtr<IO::IUserInput> input;
		static UniquePtr<IO::InputManager> inputManager;
		static UniquePtr<AUDIO::IAudioApi> audioApi;
		static UniquePtr<UuidProvider> uuidProvider;

		static UIndex64 gameFrameIndex;

	};

}

// #define OSK_USE_FORWARD_RENDERER
#define OSK_USE_DEFERRED_RENDERER
// #define OSK_USE_HYBRID_RENDERER
// #define OSK_NO_DEFAULT_RENDERER
// #define OSK_USE_GDR_RENDERER
