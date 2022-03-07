#include "OSKengine.h"

#include "Logger.h"
#include "Window.h"
#include "IRenderer.h"
#include "RenderApiType.h"
#include "Version.h"
#include "IGpuMemoryAllocator.h"
#include "GpuMemoryTypes.h"
#include "Format.h"

#include "EntityComponentSystem.h"
#include "Transform3D.h"

using namespace OSK;

TSize fps = 0;
TSize frames = 0;
float timeSinceLastFrameCount = 0.0f;

int main() {
	Engine::Create(GRAPHICS::RenderApiType::VULKAN);

	Engine::GetWindow()->Create(800, 600, "OSKengine");
	Engine::GetRenderer()->Initialize("OSKengine", {}, *Engine::GetWindow());

	// ECS Test
	Engine::GetEntityComponentSystem()->RegisterComponent<Transform3D>();

	ECS::GameObjectIndex obj0 = Engine::GetEntityComponentSystem()->SpawnObject();
	ECS::GameObjectIndex obj1 = Engine::GetEntityComponentSystem()->SpawnObject();

	Engine::GetEntityComponentSystem()->AddComponent<Transform3D>(obj0, Transform3D(obj0));
	Engine::GetEntityComponentSystem()->AddComponent<Transform3D>(obj1, Transform3D(obj1));

	Engine::GetEntityComponentSystem()->GetComponent<Transform3D>(obj1).AttachToObject(obj0);

	Engine::GetEntityComponentSystem()->GetComponent<Transform3D>(obj0).AddPosition(1.0f);
	//

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

	Engine::GetLogger()->DebugLog("FPS: " + std::to_string(fps));

	Engine::GetEntityComponentSystem()->RemoveComponent<Transform3D>(obj0);
	Engine::GetEntityComponentSystem()->DestroyObject(&obj0);
	Engine::GetEntityComponentSystem()->DestroyObject(&obj1);

	Engine::Close();

	return 0;
}
