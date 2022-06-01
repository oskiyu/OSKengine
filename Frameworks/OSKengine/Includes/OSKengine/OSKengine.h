#pragma once

#include "OSKmacros.h"

#include "UniquePtr.hpp"
#include "Version.h"
#include <string>

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

		static IO::Logger* GetLogger();
		static IO::Window* GetWindow();
		static GRAPHICS::IRenderer* GetRenderer();
		static ASSETS::AssetManager* GetAssetManager();
		static ECS::EntityComponentSystem* GetEntityComponentSystem();
		static IO::InputManager* GetInputManager();

		const static Version& GetVersion();
		const static std::string& GetBuild();

	private:

		static void RegisterBuiltinAssets();
		static void RegisterBuiltinComponents();
		static void RegisterBuiltinSystems();
		static void RegisterBuiltinVertices();

		static UniquePtr<IO::Logger> logger;
		static UniquePtr<IO::Window> window;
		static UniquePtr<GRAPHICS::IRenderer> renderer;
		static UniquePtr<ASSETS::AssetManager> assetManager;
		static UniquePtr<ECS::EntityComponentSystem> entityComponentSystem;
		static UniquePtr<IO::InputManager> inputManager;

	};

}
