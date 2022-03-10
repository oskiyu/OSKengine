#include "OSKengine.h"

#include "Logger.h"
#include "Window.h"
#include "IRenderer.h"
#include "RenderApiType.h"
#include "Version.h"
#include "IGpuMemoryAllocator.h"
#include "GpuMemoryTypes.h"
#include "Format.h"

#include "MaterialInstance.h"
#include "EntityComponentSystem.h"
#include "Transform3D.h"
#include "RenderSystem3D.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

TSize fps = 0;
TSize frames = 0;
float timeSinceLastFrameCount = 0.0f;

float deltaTime = 0.0f;

int main() {
	Engine::Create(GRAPHICS::RenderApiType::VULKAN);

	Engine::GetWindow()->Create(800, 600, "OSKengine");
	Engine::GetRenderer()->Initialize("OSKengine", {}, *Engine::GetWindow());
	//Engine::GetWindow()->SetFullScreen(true);

	Engine::GetRenderer()->PresentFrame();
	while (!Engine::GetWindow()->ShouldClose()) {
		float beginTime = Engine::GetCurrentTime();

		Engine::GetWindow()->Update();

		//TODO: Game::Update()
		

		//TODO: Game::Draw()
		Engine::GetEntityComponentSystem()->GetSystem<ECS::RenderSystem3D>()->Render(Engine::GetRenderer()->GetCommandList());

		Engine::GetRenderer()->PresentFrame();

		float endTime = Engine::GetCurrentTime();
		deltaTime = endTime - beginTime;

		frames++;
		timeSinceLastFrameCount += deltaTime;
		if (timeSinceLastFrameCount >= 1.0f) {
			fps = frames;
			frames = 0;
			timeSinceLastFrameCount = 0.0f;
		}
	}

	Engine::GetLogger()->DebugLog("FPS: " + std::to_string(fps));

	Engine::Close();

	return 0;
}
