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

int main() {
	Engine::Create(RenderApiType::OPENGL);

	Engine::GetWindow()->Create(800, 600, "XD");
	Engine::GetRenderer()->Initialize("XD", {}, *Engine::GetWindow());

	while (!Engine::GetWindow()->ShouldClose()) {
		Engine::GetWindow()->Update();

		//TODO: Game::Update()

		//TODO: Game::Draw()

		Engine::GetRenderer()->PresentFrame();
	}

	Engine::Close();

	return 0;
}
