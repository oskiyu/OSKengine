#pragma once

#include "OSKmacros.h"

#include "UniquePtr.hpp"
#include "Version.h"
#include <string>

namespace OSK {

	class Logger;
	class Window;
	class IRenderer;
	class AssetManager;
	
	enum class RenderApiType;

	class OSKAPI_CALL Engine {

	public:

		Engine() = delete;
		Engine(const Engine&) = delete;
		Engine(Engine&&) = delete;

		Engine& operator=(const Engine& e) = delete;
		Engine& operator=(Engine&& e) = delete;

		static void Create(RenderApiType type);
		static void Close();

		static float GetCurrentTime();

		static Logger* GetLogger();
		static Window* GetWindow();
		static IRenderer* GetRenderer();
		static AssetManager* GetAssetManager();

		const static Version& GetVersion();
		const static std::string& GetBuild();

	private:

		static UniquePtr<Logger> logger;
		static UniquePtr<Window> window;
		static UniquePtr<IRenderer> renderer;
		static UniquePtr<AssetManager> assetManager;

	};

}
