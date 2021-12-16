#include <stdint.h>
#include <GLFW/glfw3.h>
#include <string>

#include "Assert.h"

export module OSKengine.WindowAPI;

export import OSKengine.Types.Vector4;
export import OSKengine.RenderApiType;
export import OSKengine.Input.KeyboardState;
export import OSKengine.Input.MouseState;

import OSKengine.Logger;
import OSKengine.Assert;
import OSKengine.Memory.OwnedPtr;

export namespace OSK {

	/// <summary>
	/// Clase que representa una ventana.
	/// En esta ventana se renderizará el juego.
	/// También se encarga del input mediante teclado, ratón, mando, etc...
	/// 
	/// Esta clase debe usarse de manera estática, accediendo a ella mediante
	/// la función GetWindow().
	/// </summary>
	class Window {

		friend class RendererVulkan;

	public:

		/// <summary>
		/// Crea una instancia de una ventana.
		/// No se crea la ventana directamente, hay que llamar a Window::Create().
		/// </summary>
		Window() {
			glfwInit();
		}

		/// <summary>
		///	Destruye la ventana.
		/// </summary>
		~Window() {
			glfwTerminate();
		}

		/// <summary>
		/// Establece el api de renderizado que se va a utilizar en la ventana.
		/// </summary>
		void SetRenderApiType(RenderApiType type) {
			if (type == RenderApiType::OPENGL) {
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			}
			else {
				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			}

			renderApi = type;
		}

		/// <summary>
		/// Crea una ventana.
		/// </summary>
		/// <param name="sizeX">Tamaño horizontal en píxeles.</param>
		/// <param name="sizeY">Tamaño vertical en píxeles.</param>
		/// <param name="title">Título.</param>
		void Create(int32_t sizeX, int32_t sizeY, const std::string& title) {
			// Creación de la ventana.
			window = glfwCreateWindow(sizeX, sizeY, title.c_str(), NULL, NULL);
			OSK_ASSERT(window.HasValue(), "No se ha podido iniciar la ventana.");
			glfwMakeContextCurrent(window.GetPointer());

			this->sizeX = sizeX;
			this->sizeY = sizeY;

			UpdateScreenRatio();

			// Obtención de variables de glfw.
			glfwSetWindowUserPointer(window.GetPointer(), this);
			monitor = glfwGetPrimaryMonitor(); 
			monitorInfo = glfwGetVideoMode(monitor.GetPointer());

			// Establece los callbacks.
			glfwSetFramebufferSizeCallback(window.GetPointer(), Window::GlfwResizeCallback);
			glfwSetCursorPosCallback(window.GetPointer(), Window::GlfwMouseInputCallback);
			glfwSetScrollCallback(window.GetPointer(), Window::GlfwMouseScrollCallback);
			
			// Info-log.
			GetLogger()->InfoLog("Ventana iniciada correctamente.");
			GetLogger()->InfoLog("	Monitor sizeX: " + std::to_string(monitorInfo->width));
			GetLogger()->InfoLog("	Monitor sizeY: " + std::to_string(monitorInfo->height));
			GetLogger()->InfoLog("	Monitor refresh rate: " + std::to_string(monitorInfo->refreshRate));
		}

		/// <summary>
		/// Actualiza la ventana, procesando todos los eventos relacionados.
		/// También actualiza sus estados internos del ratón y teclado.
		/// </summary>
		void Update() {
			glfwPollEvents();

			if (renderApi == RenderApiType::OPENGL)
				glfwSwapBuffers(window.GetPointer());

			oldKeyboardState = newKeyboardState;
			UpdateKeyboardState(&newKeyboardState);

			oldMouseState = newMouseState;
			UpdateMouseState(&newMouseState);
		}

		/// <summary>
		/// Establece la ventana en modo pantalla completa (o no).
		/// </summary>
		/// <param name="fullscreen">True para entrar en pantalla completa.</param>
		void SetFullScreen(bool fullscreen) {
			if (isFullScreen == fullscreen)
				return;

			if (fullscreen) {
				// Obtenemos la posición de la ventana,
				// para poder reestablecerla después de salir de
				// la ventana completa.
				glfwGetWindowPos(window.GetPointer(), &previous.posX, &previous.posY);
				glfwGetWindowSize(window.GetPointer(), &previous.sizeX, &previous.sizeY);

				glfwSetWindowMonitor(window.GetPointer(), monitor.GetPointer(), 0, 0, monitorInfo->width, monitorInfo->height, monitorInfo->refreshRate);

				sizeX = monitorInfo->width;
				sizeY = monitorInfo->height;

				UpdateScreenRatio();
			}
			else {
				glfwSetWindowMonitor(window.GetPointer(), nullptr, previous.posX, previous.posY, previous.sizeX, previous.sizeY, monitorInfo->refreshRate);
			}

			isFullScreen = fullscreen;
		}

		/// <summary>
		/// Cambia el estado de la pantalla completa.
		/// </summary>
		void ToggleFullScreen() {
			SetFullScreen(!isFullScreen);
		}

		/// <summary>
		/// True si ahora mismo se está en pantalla completa.
		/// </summary>
		bool IsFullScreen() const {
			return isFullScreen;
		}

		/// <summary>
		/// Establece la posición del ratón, desde la esquina superior izquierda.
		/// </summary>
		/// <param name="posX">Posición, en pixeles, en X.</param>
		/// <param name="posY">Posición, en pixeles, en Y.</param>
		void SetMousePosition(int32_t posX, int32_t posY) {
			glfwSetCursorPos(window.GetPointer(), posX, posY);
		}

		/// <summary>
		/// True si la ventana va a cerrarse.
		/// 
		/// Útil para crear el bucle principal ( while (GetWindow()->ShoudlClose())
		///  { ... }.
		/// </summary>
		bool ShouldClose() const {
			return glfwWindowShouldClose(window.GetPointer());
		}

		/// <summary>
		/// Cierra la ventana.
		/// </summary>
		void Close() {
			glfwSetWindowShouldClose(window.GetPointer(), true);
		}

		/// <summary>
		/// Devuelve el tamaño de la ventana, en píxeles.
		/// </summary>
		Vector2ui GetWindowSize() const {
			return { sizeX, sizeY };
		}

		/// <summary>
		/// Devuelve el ratio de la ventana (ancho / alto).
		/// </summary>
		float GetScreenRatio() const {
			return screenRatio;
		}

		/// <summary>
		/// Devuelve la velocidad de refresco del monitor, en Hz.
		/// </summary>
		int32_t GetRefreshRate() const {
			return monitorInfo->refreshRate;
		}

		/// <summary>
		/// Devuelve el estado actual del teclado.
		/// </summary>
		const KeyboardState& GetKeyboardState() const {
			return newKeyboardState;
		}
		/// <summary>
		/// Devuelve el estado del teclado en el anterior frame.
		/// </summary>
		/// <returns></returns>
		const KeyboardState& GetPreviousKeyboardState() const {
			return oldKeyboardState;
		}

		/// <summary>
		/// Devuelve el estado actual del ratón.
		/// </summary>
		const MouseState& GetMouseState() const {
			return newMouseState;
		}
		/// <summary>
		/// Devuelve el estado del ratón del estado anterior.
		/// </summary>
		const MouseState& GetPreviousMouseState() const {
			return oldMouseState;
		}

		GLFWwindow* _GetGlfw() const {
			return window.GetPointer();
		}

	private:
		
		/// <summary>
		/// Devuelve la clase Window enlazada a la ventana de GLFW pasada.
		/// </summary>
		static Window* GetWindowForCallback(GLFWwindow* window) {
			void* ptr = glfwGetWindowUserPointer(window);
			if (auto winPtr = (Window*)ptr)
				return winPtr;

			return nullptr;
		}

		/// <summary>
		/// Llama al callback de la ventana, para el movimiento del ratón.
		/// </summary>
		static void GlfwMouseInputCallback(GLFWwindow* window, double posX, double posY) {
			GetWindowForCallback(window)->MouseInputCallback(posX, posY);
		}
		/// <summary>
		/// Callback al que se llama cuando se mueve el ratón.
		/// </summary>
		/// <param name="posX">Posición en X, en píxeles.</param>
		/// <param name="posY">Posición en Y, en píxeles.</param>
		void MouseInputCallback(double posX, double posY) {
			newMouseState._SetPosition(Vector2d(posX, posY).ToVector2i());
			newMouseState._SetRelativePosition(Vector2d(posX, posY).ToVector2f() / GetWindowSize().ToVector2f());
		}

		/// <summary>
		/// Llama al callback de la ventana, al cambiar el tamaño de la ventana.
		/// </summary>
		static void GlfwResizeCallback(GLFWwindow* window, int sizex, int sizey) {
			GetWindowForCallback(window)->ResizeCallback(sizex, sizey);
		}
		/// <summary>
		/// Callback que se llama cuando se cambia de tamaño la ventana.
		/// </summary>
		/// <param name="sizex">Resolución horizontal en píxeles.</param>
		/// <param name="sizey">Resolución vertical en píxeles.</param>
		void ResizeCallback(int sizex, int sizey) {
			sizeX = sizex; 
			sizeY = sizey;

			if (renderApi == RenderApiType::OPENGL)
				glViewport(0, 0, GetWindowSize().X, GetWindowSize().Y);
		}

		/// <summary>
		/// Llama al callback de la ventana, para el movimiento de la rueda del ratón.
		/// </summary>
		static void GlfwMouseScrollCallback(GLFWwindow* window, double dX, double dY) {
			GetWindowForCallback(window)->MouseScrollCallback(dX, dY);
		}
		/// <summary>
		/// Callback que se llama cuando se mueve la rueda del ratón.
		/// </summary>
		/// <param name="dX">Movimiento horizontal (no en todos los ratones).</param>
		/// <param name="dY">Movimiento vertical.</param>
		void MouseScrollCallback(double dX, double dY) {
			newMouseState._SetScrollX(dX);
			newMouseState._SetScrollY(dY);
		}

		void UpdateScreenRatio() {
			if (sizeY != 0)
				screenRatio = (float)sizeX / sizeY;
		}

		void UpdateKeyboardState(KeyboardState* keyboard) {
			keyboard->_SetKeyState(Key::SPACE, (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_SPACE));
			keyboard->_SetKeyState(Key::APOSTROPHE, (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_APOSTROPHE));

			for (int i = 0; i < 14; i++)
				keyboard->_SetKeyState((Key)((int)Key::COMMA + i), (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_COMMA + i));

			keyboard->_SetKeyState(Key::SEMICOLON, (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_SEMICOLON));
			keyboard->_SetKeyState(Key::EQUAL, (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_EQUAL));

			for (int i = 0; i < 29; i++)
				keyboard->_SetKeyState((Key)((int)Key::A + i), (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_A + i));

			keyboard->_SetKeyState(Key::GRAVE_ACCENT, (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_GRAVE_ACCENT));

			for (int i = 0; i < 14; i++)
				keyboard->_SetKeyState((Key)((int)Key::ESCAPE + i), (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_ESCAPE + i));

			for (int i = 0; i < 5; i++)
				keyboard->_SetKeyState((Key)((int)Key::CAPS_LOCK + i), (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_CAPS_LOCK + i));

			for (int i = 0; i < 25; i++)
				keyboard->_SetKeyState((Key)((int)Key::F1 + i), (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_F1 + i));

			for (int i = 0; i < 17; i++)
				keyboard->_SetKeyState((Key)((int)Key::KEYPAD_0 + i), (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_KP_0 + i));

			for (int i = 0; i < 9; i++)
				keyboard->_SetKeyState((Key)((int)Key::LEFT_SHIFT + i), (KeyState)glfwGetKey(window.GetPointer(), GLFW_KEY_LEFT_SHIFT + i));
		}

		void UpdateMouseState(MouseState* mouse) {
			for (int i = 0; i < 8; i++)
				mouse->_SetButtonState((MouseButton)i, (ButtonState)glfwGetMouseButton(window.GetPointer(), i));

			mouse->_SetButtonState(MouseButton::BUTTON_LEFT, mouse->GetButtonState(MouseButton::BUTTON_1));
			mouse->_SetButtonState(MouseButton::BUTTON_RIGHT, mouse->GetButtonState(MouseButton::BUTTON_2));
			mouse->_SetButtonState(MouseButton::BUTTON_MIDDLE, mouse->GetButtonState(MouseButton::BUTTON_3));
		}

		RenderApiType renderApi;

		KeyboardState newKeyboardState;
		KeyboardState oldKeyboardState;

		MouseState newMouseState;
		MouseState oldMouseState;

		uint32_t sizeX = 0;
		uint32_t sizeY = 0;
		float screenRatio = 0.0f;

		bool isFullScreen = false;

		/// <summary>
		/// Tamaño y posición de la ventana, antes de ponerse en pantalla completa.
		/// </summary>
		struct {
			int32_t sizeX = 0;
			int32_t sizeY = 0;

			int32_t posX = 0;
			int32_t posY = 0;
		} previous;

		OwnedPtr<GLFWwindow> window = nullptr;
		OwnedPtr<GLFWmonitor> monitor = nullptr; // Para pantalla completa.
		const GLFWvidmode* monitorInfo = nullptr;
	};

}
