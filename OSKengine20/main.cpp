import OSKengine.WindowAPI;
import OSKengine.Logger;
import OSKengine.File;

import OSKengine;

#include <stdint.h>
#include <string>
#include <iostream>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>
#include <set>

using namespace OSK;

int main() {
	Engine::GetLogger()->Start("LOG_LAST.txt");

	Engine::GetWindow()->SetRenderApiType(RenderApiType::DX12);
	//CreateRenderer(RenderApiType::DX12);
	//GetRendererRef() = new RendererVulkan();
	//renderer = new RendererDx12();
	//GetWindow()->SetRenderApiType(RenderApiType::OPENGL); renderer = new RendererOgl();

	Engine::GetWindow()->Create(800, 600, "XD");
	Engine::GetRenderer()->Initialize("", {0, 0 ,0}, *Engine::GetWindow());
	while (!Engine::GetWindow()->ShouldClose()) {
		Engine::GetWindow()->Update();


	}

	Engine::GetRenderer()->Close();
	Engine::GetLogger()->Close();

	return 0;
}
