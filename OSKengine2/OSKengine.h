#pragma once

#include "OSKmacros.h"

#include "UniquePtr.hpp"
#include "Version.h"
#include <string>

namespace OSK {

	class Logger;
	class Window;
	class IRenderer;
	
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

		static Logger* GetLogger();
		static Window* GetWindow();
		static IRenderer* GetRenderer();

		const static Version& GetVersion();
		const static std::string& GetBuild();

	private:

		static UniquePtr<Logger> logger;
		static UniquePtr<Window> window;
		static UniquePtr<IRenderer> renderer;

	};

}
