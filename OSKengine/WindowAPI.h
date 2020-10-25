#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "KeyboardState.h"
#include "MouseMovementInputModeEnum.h"
#include "MouseState.h"
#include "MouseVisibilityEnum.h"
#include "OverallMouseInputModeEnum.h"
#include "GraphicsAPIenum.h"

#include <glad/glad.h>
#include <glfw3.h>

#include <string>

namespace OSK {

	namespace UI {
		class BaseUIElement;
	}


	//Clase para la creación y uso de una ventana.
	class OSKAPI_CALL WindowAPI {

	public:
		GraphicsAPI UsedGraphicsAPI = GraphicsAPI::OPENGL;

		//Crea una instancia WindowAPI.
		WindowAPI();


		//Destruye la WindowAPI.
		~WindowAPI();


		//Crea una ventana con tamaño { sizeX, sizeY } y con título { name }.
		//Retorna 'false' si hay un error.
		result_t SetWindow(const int32_t& sizeX, const int32_t& sizeY, const std::string& name, const GraphicsAPI& graphicsAPI);

		//void SetWindowIcon(const std::string& iconPath);

		//Carga OpenGL.
		//Retorna 'false' si hay un error.
		result_t LoadOpenGL();


		//Establece la versión de OpenGL a utilizar (3.3 por defecto).
		void SetOpenGLVersion(const int32_t& mayor, const int32_t& menor);


		//Establece la versión principal de OpenGL (3 por defecto).
		void SetMayorOpenGLVersion(const int32_t& version);


		//Establece la revisión de OpenGL (3 por defecto).
		void SetMinorOpenGLVersion(const int32_t& version);


		//Actualiza la ventana y retorna true si el programa ha de finalizar.
		bool WindowShouldClose();


		//Intercambia los buffers de la ventana.
		void SwapBuffers();


		//Obtiene y actualiza el input (teclado/ratón, etc.).
		void PollEvents();


		//Actualiza el estado de un KeyboardState dado.
		void UpdateKeyboardState(KeyboardState& keyboard);


		//Actualiza el estado de un MouseState dado.
		void UpdateMouseState(MouseState& mouseState);


		//Establece la posición del cursor, en píxeles.
		void SetMousePosition(const int32_t& posX, const int32_t& posY);


		//Establece el modo del cursor.
		//MouseInputMode::NORMAL: normal.
	    //MouseInputMode::ALWAYS_RETURN: el ratón siempre se mantiene en el centro de la pantalla y es invisible.
		void SetMouseMode(const MouseInputMode& mode);


		//Establece la precisión del mouse.
		//WINDOWS: sin 'raw mouse option', con 'precisión del puntero'.
		//RAW: sin 'precisión del puntero'.
		void SetMouseMovementMode(const MouseMovementMode& mode);


		//Establece la visibilidad del cursor.
		void SetMouseVisibility(const MouseVisibility& mode);


		//Establece la pantalla completa (o no).
		void SetFullscreen(const bool& fullscreen);


		//Cierra la ventana.
		void Close();


		//TODO:
		void SetMousePosition(const uint32_t& x, const uint32_t& y);


		void CenterMouse();


		void SetUserInterface(UI::BaseUIElement* ui);


		Vector4 GetRectangle() const;


		//Obtiene el tiempo actual.
		deltaTime_t GetTime();


		//Tamaño de la ventana en el eje X (ancho).
		//En píxeles.
		int32_t ScreenSizeX;

		//Tamaño de la ventana en el eje Y (alto).
		//En píxeles.
		int32_t ScreenSizeY;


		//Ratio de aspecto de la ventana.
		//En porcentaje de X respecto a Y (x/y).
		float_t ScreenRatio;


		//TODO:
		UI::BaseUIElement* UserInterface = nullptr;


		//GLFW.
		GLFWwindow* GetGLFWWindow() const;


		//True si está en pantalla completa
		bool IsFullscreen = false;


		//Si es true, el viewport debe ser actualizado.
		bool ViewportShouldBeUpdated;

	private:

		GLFWwindow* window = nullptr;

		GLFWmonitor* monitor = nullptr;
		
		int32_t openGlVersionMayor = 3;

		int32_t openGlVersionMinor = 3;

		int32_t posXbackup;

		int32_t posYbackup;

		int32_t sizeXbackup;

		int32_t sizeYbackup;

		MouseState mouseState;

		void updateMouseButtons();

		static void mouseInputCallback(GLFWwindow*, double, double);

		void mouseInput(double_t, double_t);

		static void mouseScrollCallback(GLFWwindow*, double, double);

		void mouseScroll(double_t, double_t);

		static void defaultFramebufferSizeCallback(GLFWwindow*, int, int);

		void resize(double_t, double_t);

		//TODO:
		void updateUI(UI::BaseUIElement* element);

	};


	
	
}