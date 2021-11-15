#include "OSKengine.h"

#include "Logger.h"
#include "Window.h"
#include "IRenderer.h"
#include "RenderApiType.h"
#include "Version.h"

using namespace OSK;

int main() {
	Engine::Create(RenderApiType::VULKAN);

	Engine::GetWindow()->Create(800, 600, "XD");
	Engine::GetRenderer()->Initialize("XD", {}, *Engine::GetWindow());

	while (!Engine::GetWindow()->ShouldClose()) {
		Engine::GetWindow()->Update();
	}

	Engine::Close();

	return 0;
}
