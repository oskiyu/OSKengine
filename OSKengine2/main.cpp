#include "OSKengine.h"

#include "Logger.h"
#include "Window.h"
#include "IRenderer.h"
#include "RenderApiType.h"
#include "Version.h"
#include "IGpuMemoryAllocator.h"
#include "GpuMemoryTypes.h"
#include "Format.h"

using namespace OSK;

TSize fps = 0;
TSize frames = 0;
float timeSinceLastFrameCount = 0.0f;

int main() {
	Engine::Create(RenderApiType::VULKAN);

	Engine::GetWindow()->Create(800, 600, "OSKengine");
	Engine::GetRenderer()->Initialize("OSKengine", {}, *Engine::GetWindow());

	while (!Engine::GetWindow()->ShouldClose()) {
		float beginTime = Engine::GetCurrentTime();

		Engine::GetWindow()->Update();

		//TODO: Game::Update()

		//TODO: Game::Draw()

		Engine::GetRenderer()->PresentFrame();

		float endTime = Engine::GetCurrentTime();
		float deltaTime = endTime - beginTime;

		frames++;
		timeSinceLastFrameCount += deltaTime;
		if (timeSinceLastFrameCount >= 1.0f) {
			fps = frames;
			frames = 0;
			timeSinceLastFrameCount = 0.0f;
		}
	}

	Engine::Close();

	return 0;
}
