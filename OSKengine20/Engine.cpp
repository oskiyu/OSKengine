import OSKengine;

using namespace OSK;

Window Engine::window;
UniquePtr<IRenderer> Engine::renderer = nullptr;
Logger Engine::logger;

Window* Engine::GetWindow() {
	return &window;
}

IRenderer* Engine::GetRenderer() {
	return renderer.GetPointer();
}

Logger* Engine::GetLogger() {
	return &logger;
}
