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
#include "IUserInput.h"
#include "InputManager.h"

namespace OSK {
	
	namespace IO {
		class Logger;
		class IDisplay;
		class InputManager;
	}

	namespace GRAPHICS {
		class IRenderer;
		enum class RenderApiType;
	}

	namespace ASSETS {
		class AssetManager;
	}
	namespace ECS {
		class EntityComponentSystem;
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

		/// @return Devuelve la versión del motor. 
		static Version GetVersion();
		/// @return Devuelve la build del motor, en formato aaaa.mm.dd.n
		const static std::string& GetBuild();

		static void RegisterBuiltinAssets();
		static void RegisterBuiltinComponents();
		static void RegisterBuiltinSystems();
		static void RegisterBuiltinEvents();
		static void RegisterBuiltinVertices();

	private:

		static UniquePtr<IO::Logger> logger;
		static UniquePtr<IO::IDisplay> display;
		static UniquePtr<GRAPHICS::IRenderer> renderer;
		static UniquePtr<ASSETS::AssetManager> assetManager;
		static UniquePtr<ECS::EntityComponentSystem> entityComponentSystem;
		static UniquePtr<IO::IUserInput> input;
		static UniquePtr<IO::InputManager> inputManager;

	};

}

#define OSK_USE_FORWARD_RENDERER
// #define OSK_USE_DEFERRED_RENDERER
// #define OSK_USE_HYBRID_RENDERER
// #define OSK_NO_DEFAULT_RENDERER
