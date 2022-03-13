#pragma once

#include "Vector2.hpp"
#include "OwnedPtr.h"
#include "UniquePtr.hpp"

#include <string>

struct GLFWwindow;
struct GLFWmonitor;
struct GLFWvidmode;

namespace OSK::GRAPHICS {
	enum class RenderApiType;
}

namespace OSK::IO {

	class KeyboardState;
	class MouseState;

	enum class MouseReturnMode;
	enum class MouseMotionMode;

	/// <summary>
	/// Clase que representa una ventana.
	/// En esta ventana se renderizará el juego.
	/// También se encarga del input mediante teclado, ratón, mando, etc...
	/// 
	/// Esta clase debe usarse de manera estática, accediendo a ella mediante
	/// la función GetWindow().
	/// </summary>
	class OSKAPI_CALL Window {

	public:

		/// <summary>
		/// Crea una instancia de una ventana.
		/// No se crea la ventana directamente, hay que llamar a Window::Create().
		/// </summary>
		Window();

		/// <summary>
		///	Destruye la ventana.
		/// </summary>
		~Window();

		/// <summary>
		/// Establece el api de renderizado que se va a utilizar en la ventana.
		/// </summary>
		void SetRenderApiType(GRAPHICS::RenderApiType type);

		/// <summary>
		/// Crea una ventana.
		/// </summary>
		/// <param name="sizeX">Tamaño horizontal en píxeles.</param>
		/// <param name="sizeY">Tamaño vertical en píxeles.</param>
		/// <param name="title">Título.</param>
		void Create(int32_t sizeX, int32_t sizeY, const std::string& title);

		/// <summary>
		/// Actualiza la ventana, procesando todos los eventos relacionados.
		/// También actualiza sus estados internos del ratón y teclado.
		/// </summary>
		void Update();

		/// <summary>
		/// Actualiza los valores del mouse y del frame anterior.
		/// </summary>
		void UpdateMouseAndKeyboardOldStates();

		/// <summary>
		/// Establece la ventana en modo pantalla completa (o no).
		/// </summary>
		/// <param name="fullscreen">True para entrar en pantalla completa.</param>
		void SetFullScreen(bool fullscreen);

		/// <summary>
		/// Cambia el estado de la pantalla completa.
		/// </summary>
		void ToggleFullScreen();

		/// <summary>
		/// True si ahora mismo se está en pantalla completa.
		/// </summary>
		bool IsFullScreen() const;

		/// <summary>
		/// Establece la posición del ratón, desde la esquina superior izquierda.
		/// </summary>
		/// <param name="posX">Posición, en pixeles, en X.</param>
		/// <param name="posY">Posición, en pixeles, en Y.</param>
		void SetMousePosition(TSize posX, TSize posY);

		/// <summary>
		/// Establece si el ratón puede moverse libremente o si
		/// volverá siempre al centro de la pantalla, para controlar
		/// la cámara.
		/// </summary>
		void SetMouseReturnMode(MouseReturnMode mode);		
		
		/// <summary>
		/// Establece si el ratón puede moverse libremente o si
		/// volverá siempre al centro de la pantalla, para controlar
		/// la cámara.
		/// </summary>
		void SetMouseMotionMode(MouseMotionMode mode);

		/// <summary>
		/// True si la ventana va a cerrarse.
		/// 
		/// Útil para crear el bucle principal ( while (GetWindow()->ShoudlClose())
		///  { ... }.
		/// </summary>
		bool ShouldClose() const;

		/// <summary>
		/// Cierra la ventana.
		/// </summary>
		void Close();

		/// <summary>
		/// Devuelve el tamaño de la ventana, en píxeles.
		/// </summary>
		Vector2ui GetWindowSize() const;

		/// <summary>
		/// Devuelve el ratio de la ventana (ancho / alto).
		/// </summary>
		float GetScreenRatio() const;

		/// <summary>
		/// Devuelve la velocidad de refresco del monitor, en Hz.
		/// </summary>
		int32_t GetRefreshRate() const;

		/// <summary>
		/// Devuelve el estado actual del teclado.
		/// </summary>
		const KeyboardState* GetKeyboardState() const;
		/// <summary>
		/// Devuelve el estado del teclado en el anterior frame.
		/// </summary>
		/// <returns></returns>
		const KeyboardState* GetPreviousKeyboardState() const;

		/// <summary>
		/// Devuelve el estado actual del ratón.
		/// </summary>
		const MouseState* GetMouseState() const;
		/// <summary>
		/// Devuelve el estado del ratón del estado anterior.
		/// </summary>
		const MouseState* GetPreviousMouseState() const;

		/// <summary>
		/// Obtiene el handler de la ventana, de GLFW.
		/// </summary>
		GLFWwindow* _GetGlfw() const;

		/// <summary>
		/// Devuelve true si la ventana ha sido creada y está abierta.
		/// </summary>
		bool IsOpen() const;

	private:

		/// <summary>
		/// Devuelve la clase Window enlazada a la ventana de GLFW pasada.
		/// </summary>
		static Window* GetWindowForCallback(GLFWwindow* window);

		/// <summary>
		/// Llama al callback de la ventana, para el movimiento del ratón.
		/// </summary>
		static void GlfwMouseInputCallback(GLFWwindow* window, double posX, double posY);
		/// <summary>
		/// Callback al que se llama cuando se mueve el ratón.
		/// </summary>
		/// <param name="posX">Posición en X, en píxeles.</param>
		/// <param name="posY">Posición en Y, en píxeles.</param>
		void MouseInputCallback(double posX, double posY);

		/// <summary>
		/// Llama al callback de la ventana, al cambiar el tamaño de la ventana.
		/// </summary>
		static void GlfwResizeCallback(GLFWwindow* window, int sizex, int sizey);
		/// <summary>
		/// Callback que se llama cuando se cambia de tamaño la ventana.
		/// </summary>
		/// <param name="sizex">Resolución horizontal en píxeles.</param>
		/// <param name="sizey">Resolución vertical en píxeles.</param>
		void ResizeCallback(int sizex, int sizey);

		/// <summary>
		/// Llama al callback de la ventana, para el movimiento de la rueda del ratón.
		/// </summary>
		static void GlfwMouseScrollCallback(GLFWwindow* window, double dX, double dY);
		/// <summary>
		/// Callback que se llama cuando se mueve la rueda del ratón.
		/// </summary>
		/// <param name="dX">Movimiento horizontal (no en todos los ratones).</param>
		/// <param name="dY">Movimiento vertical.</param>
		void MouseScrollCallback(double dX, double dY);

		/// <summary>
		/// Actualuza el valor de la variable screenRatio.
		/// </summary>
		void UpdateScreenRatio();

		/// <summary>
		/// Actualiza el estado de las teclas del teclado,
		/// para el KeyboardState dado.
		/// </summary>
		void UpdateKeyboardState(KeyboardState* keyboard);

		/// <summary>
		/// Actualiza el estado del ratón dado.
		/// </summary>
		void UpdateMouseState(MouseState* mouse);

		/// <summary>
		/// API gráfica usada.
		/// 
		/// Si es OpenGL, la ventana se encarga de 
		/// hacer el swap buffeers.
		/// </summary>
		GRAPHICS::RenderApiType renderApi;

		UniquePtr<KeyboardState> newKeyboardState;
		UniquePtr<KeyboardState> oldKeyboardState;

		UniquePtr<MouseState> newMouseState;
		UniquePtr<MouseState> oldMouseState;

		uint32_t sizeX = 0;
		uint32_t sizeY = 0;
		float screenRatio = 0.0f;

		bool isFullScreen = false;
		bool isOpen = false;

		/// <summary>
		/// Tamaño y posición de la ventana, antes de ponerse en pantalla completa.
		/// </summary>
		struct {
			int32_t sizeX = 0;
			int32_t sizeY = 0;

			int32_t posX = 0;
			int32_t posY = 0;
		} previous;

		// ---------------- GLFW --------------------- //

		OwnedPtr<GLFWwindow> window = nullptr;
		OwnedPtr<GLFWmonitor> monitor = nullptr; // Para pantalla completa.
		const GLFWvidmode* monitorInfo = nullptr;

	};

}
