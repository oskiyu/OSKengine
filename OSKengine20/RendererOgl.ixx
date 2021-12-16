#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#include "Assert.h"

export module OSKengine.Graphics.RendererOgl;

export import OSKengine.Graphics.IRenderer;

import OSKengine.Logger;
import OSKengine.Assert;

export namespace OSK {

	class RendererOgl : public IRenderer {

	public:

		void Initialize(const std::string& appName, const Version& version, const Window& window) override {
			auto gladResult = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
			OSK_ASSERT(gladResult != 0, "No se ha podido cargar OpenGL.");

			glViewport(0, 0, window.GetWindowSize().X, window.GetWindowSize().Y);

			GetLogger()->InfoLog("Iniciado OpenGL.");
		}

		void Close() override {
			GetLogger()->InfoLog("Destruyendo el renderizador de OpenGL.");
		}

	private:

	};

}
