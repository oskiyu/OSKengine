module;

#include <OSKengine/KeyboardState.h>
#include <OSKengine/MouseState.h>
#include <OSKengine/Window.h>
#include <OSKengine/OSKengine.h>
#include <OSKengine/IInputListener.h>
#include <OSKengine/Logger.h>
#include <OSKengine/IRenderer.h>
#include <OSKengine/IGpuMemoryAllocator.h>

export module GameInputListener;

using namespace OSK;
using namespace OSK::IO;

export class GameInputListener : public IInputListener {

public:

	void OnKeyStroke(Key key) override {
		switch (key) {

		case Key::ESCAPE:
			Engine::GetWindow()->Close();
			break;

		case Key::F11:
			Engine::GetWindow()->ToggleFullScreen();
			break;

		case Key::Q:
			/*Engine::GetLogger()->Log(LogLevel::INFO, "GPU memory allocator stats: ");
			Engine::GetLogger()->Log(LogLevel::INFO, "		Total allocations: " + std::to_string(Engine::GetRenderer()->GetMemoryAllocator()->stats.totalMemoryAllocations));
			Engine::GetLogger()->Log(LogLevel::INFO, "		Total sub allocations: " + std::to_string(Engine::GetRenderer()->GetMemoryAllocator()->stats.totalSubAllocations));
			Engine::GetLogger()->Log(LogLevel::INFO, "		Total reserved size: " + std::to_string(Engine::GetRenderer()->GetMemoryAllocator()->stats.totalReservedSize));
			Engine::GetLogger()->Log(LogLevel::INFO, "		Total used size: " + std::to_string(Engine::GetRenderer()->GetMemoryAllocator()->stats.totalUsedSize));
			*/
			break;
		}
	}

};
