#include "AndroidDisplay.h"

#ifdef OSK_ANDROID

#include <android/native_window.h>
#include <android_native_app_glue.h>

#include "OSKengine.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::IO;

AndroidWindow::AndroidWindow(android_app* app) : m_app(app) {
	
}

void AndroidWindow::Create(Vector2ui resolution, const std::string& title) {
	this->resolution = {
		static_cast<USize32>(ANativeWindow_getWidth(m_app->window)),
		static_cast<USize32>(ANativeWindow_getHeight(m_app->window))
	};

	refreshRate = 30; // ???
	isOpen = true;

	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "Resolución: ", resolution.x, "x", resolution.y);
	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "VENTANA: ", (USize64)m_app->window);
}

void AndroidWindow::Update() {

}

void AndroidWindow::Close() {
	isOpen = false;
}

void AndroidWindow::QueryInterface(TInterfaceUuid interfaceUuid, void** ptr) const {
	*ptr = nullptr;
}

void AndroidWindow::QueryConstInterface(TInterfaceUuid uuid, const void** ptr) const {
	*ptr = nullptr;
}

ANativeWindow* AndroidWindow::GetNativeWindow() {
	return m_app->window;
}

const ANativeWindow* AndroidWindow::GetNativeWindow() const {
	return m_app->window;
}

#endif // OSK_ANDROID
