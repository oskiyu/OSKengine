#pragma once

#include "OSKmacros.h"

#include "UniquePtr.hpp"
#include "Version.h"
#include <string>

#include "Logger.h"
#include "Window.h"
#include "IRenderer.h"
#include "AssetManager.h"
#include "EntityComponentSystem.h"
#include "InputManager.h"

namespace OSK {

	namespace IO {
		class Logger;
		class Window;
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
	

	/// <summary>
	/// Clase principal de OSKengine.
	/// </summary>
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


		/// <summary> Devuelve un puntero al logger del motor. </summary>
		/// @note El puntero es estable.
		/// @warning Será nullptr hasta que no se llame a Engine::Create.
		static IO::Logger* GetLogger();

		/// <summary> Devuelve un puntero a la ventana del juego. </summary>
		/// @note El puntero es estable.
		/// @warning Será nullptr hasta que no se llame a Engine::Create.
		static IO::Window* GetWindow();

		/// <summary> Devuelve un puntero al renderizador del motor. </summary>
		/// @note El puntero es estable.
		/// @warning Será nullptr hasta que no se llame a Engine::Create.
		/// @warning El renderizador no será usable hasta que no se llame a IRenderer::Initialize.
		static GRAPHICS::IRenderer* GetRenderer();

		/// <summary> Devuelve un puntero al sistema de assets del motor. </summary>
		/// @note El puntero es estable.
		/// @warning Será nullptr hasta que no se llame a Engine::Create.
		static ASSETS::AssetManager* GetAssetManager();

		[[deprecated("Usar Engine::GetEcs()")]]
		static ECS::EntityComponentSystem* GetEntityComponentSystem();

		/// <summary> Devuelve un puntero al ECS del motor. </summary>
		/// @note El puntero es estable.
		/// @warning Será nullptr hasta que no se llame a Engine::Create.
		static ECS::EntityComponentSystem* GetEcs();

		/// <summary> Devuelve un puntero al sistema de input del motor. </summary>
		/// @note El puntero es estable.
		/// @warning Será nullptr hasta que no se llame a Engine::Create.
		static IO::InputManager* GetInputManager();

		/// <summary> Devuelve la versión del motor. </summary>
		const static Version& GetVersion();
		/// <summary> Devuelve la build del motor, en formato aaaa.mm.dd.n </summary>
		const static std::string& GetBuild();

		static void RegisterBuiltinAssets();
		static void RegisterBuiltinComponents();
		static void RegisterBuiltinSystems();
		static void RegisterBuiltinVertices();

	private:

		static UniquePtr<IO::Logger> logger;
		static UniquePtr<IO::Window> window;
		static UniquePtr<GRAPHICS::IRenderer> renderer;
		static UniquePtr<ASSETS::AssetManager> assetManager;
		static UniquePtr<ECS::EntityComponentSystem> entityComponentSystem;
		static UniquePtr<IO::InputManager> inputManager;

	};

}
