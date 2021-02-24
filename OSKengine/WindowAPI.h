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

#include <glad/glad.h>
#include <glfw3.h>

#include <string>

namespace OSK {

	namespace UI {
		class BaseUIElement;
	}


	//Clase para la creaci�n y uso de una ventana.
	class OSKAPI_CALL WindowAPI {

	public:

		//Crea una instancia WindowAPI.
		WindowAPI();

		//Destruye la WindowAPI.
		~WindowAPI();

		//Crea una ventana con tama�o { sizeX, sizeY } y con t�tulo { name }.
		//Retorna 'false' si hay un error.
		result_t SetWindow(int32_t sizeX, int32_t sizeY, const std::string& name);

		//Actualiza la ventana y retorna true si el programa ha de finalizar.
		bool WindowShouldClose();

		//Intercambia los buffers de la ventana.
		void SwapBuffers();

		//Obtiene y actualiza el input (teclado/rat�n, etc.).
		void PollEvents();

		//Actualiza el estado de un KeyboardState dado.
		void UpdateKeyboardState(KeyboardState& keyboard);

		//Actualiza el estado de un MouseState dado.
		void UpdateMouseState(MouseState& mouseState);

		//Establece la posici�n del cursor, en p�xeles.
		void SetMousePosition(int32_t posX, int32_t posY);

		//Establece el modo del cursor.
		//MouseInputMode::NORMAL: normal.
	    //MouseInputMode::ALWAYS_RETURN: el rat�n siempre se mantiene en el centro de la pantalla y es invisible.
		void SetMouseMode(MouseInputMode mode);

		//Establece la precisi�n del mouse.
		//WINDOWS: sin 'raw mouse option', con 'precisi�n del puntero'.
		//RAW: sin 'precisi�n del puntero'.
		void SetMouseMovementMode(MouseMovementMode mode);

		//Establece la visibilidad del cursor.
		void SetMouseVisibility(MouseVisibility mode);

		//Establece la pantalla completa (o no).
		void SetFullscreen(bool fullscreen);

		//Cierra la ventana.
		void Close();

		//TODO:
		void SetMousePosition(uint32_t x, uint32_t y);

		void CenterMouse();

		Vector4 GetRectangle() const;

		//Obtiene el tiempo actual.
		deltaTime_t GetTime();

		//Tama�o de la ventana en el eje X (ancho).
		//En p�xeles.
		int32_t ScreenSizeX;

		//Tama�o de la ventana en el eje Y (alto).
		//En p�xeles.
		int32_t ScreenSizeY;


		//Ratio de aspecto de la ventana.
		//En porcentaje de X respecto a Y (x/y).
		float_t ScreenRatio;

		//TODO:
		UI::BaseUIElement* UserInterface = nullptr;

		//GLFW.
		GLFWwindow* GetGLFWWindow() const;

		//True si est� en pantalla completa
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

	};
		
}