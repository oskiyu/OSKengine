#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "KeyboardState.h"
#include "MouseAccelerationModeEnum.h"
#include "MouseState.h"
#include "MouseVisibilityEnum.h"
#include "OverallMouseInputModeEnum.h"

#include <GLFW/glfw3.h>

#include <string>

namespace OSK {

	namespace UI {

		class BaseUIElement;

	}


	/// <summary>
	/// Clase para la creaci�n y uso de una ventana.
	/// </summary>
	class OSKAPI_CALL WindowAPI {

		friend class RenderAPI;

	public:

		/// <summary>
		/// Crea una instancia WindowAPI.
		/// </summary>
		WindowAPI();

		/// <summary>
		/// Destruye la WindowAPI.
		/// </summary>
		~WindowAPI();

		//Crea una ventana con tama�o { sizeX, sizeY } y con t�tulo { name }.
		//Retorna 'false' si hay un error.

		/// <summary>
		/// Crea una ventana.
		/// </summary>
		/// <param name="sizeX">Tama�o en X.</param>
		/// <param name="sizeY">Tama�o en Y.</param>
		/// <param name="name">T�tulo de la ventana.</param>
		/// <returns>Retorna 'false' si hay un error.</returns>
		result_t SetWindow(int32_t sizeX, int32_t sizeY, const std::string& name);

		/// <summary>
		/// Actualiza la ventana y retorna true si el programa ha de finalizar.
		/// </summary>
		/// <returns>True si el programa ha de finalizar.</returns>
		bool WindowShouldClose();

		/// <summary>
		/// Obtiene y actualiza el input (teclado/rat�n, etc...).
		/// </summary>
		void PollEvents();

		/// <summary>
		/// Actualiza el estado de un KeyboardState dado.
		/// </summary>
		/// <param name="keyboard">Estado a actualizar.</param>
		void UpdateKeyboardState(KeyboardState& keyboard);

		/// <summary>
		/// Actualiza el estado de un MouseState dado.
		/// </summary>
		/// <param name="mouseState">MouseState a actualizar.</param>
		void UpdateMouseState(MouseState& mouseState);

		/// <summary>
		/// Establece la posici�n del cursor, en p�xeles.
		/// </summary>
		/// <param name="posX">Posici�n X desde la esquina superior izquierda.</param>
		/// <param name="posY">Posici�n Y desde la esquina superior izquierda.</param>
		void SetMousePosition(int32_t posX, int32_t posY);

		/// <summary>
		/// Establece el modo del cursor.
		/// </summary>
		/// <param name="mode">Modo de input del cursor.</param>
		void SetMouseMode(MouseInputMode mode);

		/// <summary>
		/// Establece la precisi�n del mouse.
		/// </summary>
		/// <param name="mode">Precisi�n del mouse.</param>
		void SetMouseMovementMode(MouseAccelerationMode mode);

		/// <summary>
		/// Establece la visibilidad del cursor.
		/// </summary>
		/// <param name="mode">Nueva visibilidad.</param>
		void SetMouseVisibility(MouseVisibility mode);

		/// <summary>
		/// Establece la pantalla completa.
		/// </summary>
		/// <param name="fullscreen">True si es pantalla completa.</param>
		void SetFullscreen(bool fullscreen);

		/// <summary>
		/// Cierra la ventana.
		/// </summary>
		void Close();

		/// <summary>
		/// Centra el mouse en el centro de la ventana.
		/// </summary>
		void CenterMouse();

		/// <summary>
		/// Devuelve un rect�ngulo con el tama�o de la ventana.
		/// </summary>
		/// <returns>Tama�o de la ventana.</returns>
		Vector4 GetRectangle() const;

		/// <summary>
		/// Obtiene el tiempo actual.
		/// </summary>
		/// <returns>Tiempo actual.</returns>
		deltaTime_t GetTime();

		/// <summary>
		/// Tama�o de la ventana.
		/// </summary>
		Vector2i GetSize() const;

		/// <summary>
		/// Ratio de aspecto de la ventana.
		/// En porcentaje de X respecto a Y (x/y).
		/// </summary>
		float GetScreenAspectRatio() const;

		/// <summary>
		/// True si est� en pantalla completa.
		/// </summary>
		bool IsFullscreen() const;

		
	private:

		/// <summary>
		/// Ventana nativa.
		/// </summary>
		GLFWwindow* window = nullptr;

		/// <summary>
		/// Monitor (para modo panattlla completa).
		/// </summary>
		GLFWmonitor* monitor = nullptr;
		
		/// <summary>
		/// Posici�n en X antes de ponerse a pantalla completa.
		/// </summary>
		int32_t posXbackup;

		/// <summary>
		/// Posici�n en T antes de ponerse a pantalla completa.
		/// </summary>
		int32_t posYbackup;

		/// <summary>
		/// Tama�o en X antes de ponerse a pantalla completa.
		/// </summary>
		int32_t sizeXbackup;

		/// <summary>
		/// Tama�o en Y antes de ponerse a pantalla completa.
		/// </summary>
		int32_t sizeYbackup;

		/// <summary>
		/// Estado del rat�n.
		/// </summary>
		MouseState mouseState;

		/// <summary>
		/// Actualiza los estados del rat�n.
		/// </summary>
		void updateMouseButtons();

		/// <summary>
		/// Funci�n que se ejecuta al moverse el rat�n.
		/// </summary>
		static void mouseInputCallback(GLFWwindow*, double, double);

		/// <summary>
		/// Funci�n que se ejecuta al moverse el rat�n.
		/// </summary>
		void mouseInput(double_t, double_t);

		/// <summary>
		/// Funci�n que se ejecuta al moverse la rueda del rat�n.
		/// </summary>
		static void mouseScrollCallback(GLFWwindow*, double, double);

		/// <summary>
		/// Funci�n que se ejecuta al moverse la rueda del rat�n.
		/// </summary>
		void mouseScroll(double_t, double_t);
		
		/// <summary>
		/// Funci�n que se ejecuta al cambiar de tama��o la ventana.
		/// </summary>
		static void defaultFramebufferSizeCallback(GLFWwindow*, int, int);

		/// <summary>
		/// Funci�n que se ejecuta al cambiar de tama�o la ventana.
		/// </summary>
		void resize(double_t, double_t);
		
		/// <summary>
		/// Si es true, el viewport debe ser actualizado.
		/// </summary>
		bool viewportShouldBeUpdated;

		/// <summary>
		/// Tama�o de la ventana en el eje X (ancho).
		/// </summary>
		int32_t screenSizeX;

		/// <summary>
		/// Tama�o de la ventana en el eje Y (alto).
		/// </summary>
		int32_t screenSizeY;

		/// <summary>
		/// Ratio de aspecto de la ventana.
		/// En porcentaje de X respecto a Y (x/y).
		/// </summary>
		float_t screenRatio;

		/// <summary>
		/// True si est� en pantalla completa.
		/// </summary>
		bool isFullscreen = false;

	};
		
}