#pragma once

#include "Vector2.hpp"
#include "UniquePtr.hpp"

#include <string>

#include "IDisplay.h"
#include "IFullscreenableDisplay.h"

struct GLFWwindow;
struct GLFWmonitor;
struct GLFWvidmode;

namespace OSK::IO {

	/// @brief Clase que representa una ventana.
	/// En esta ventana se renderizará el juego.
	/// También se encarga del input mediante teclado, ratón, mando, etc...
	/// 
	/// @note Esta clase debe usarse de manera estática, accediendo a ella mediante
	/// la función GetWindow().
	class OSKAPI_CALL Window final : public IDisplay, public IFullscreenableDisplay {

	public:

		/// @brief Inicia GLFW.
		/// @warning No se crea la ventana directamente, hay que llamar a Window::Create().
		Window();

		/// @brief Destruye la ventana.
		~Window();


		void QueryInterface(TInterfaceUuid uuid, void** ptr) const override;
		void QueryConstInterface(TInterfaceUuid uuid, const void** ptr) const override;


		void Create(Vector2ui size, const std::string& title) override;


		/// @brief Actualiza la ventana, procesando todos los eventos relacionados.
		void Update() override;

		void SetFullscreen(bool fullscreen) override;


		/// @brief Cierra la ventana.
		void Close() override;



		/// @return GLFW
		GLFWwindow* _GetGlfw();


		static void ShowMessageBox(std::string_view message);

	private:

		/// @return Clase Window enlazada a la ventana de GLFW pasada.
		static Window* GetWindowForCallback(GLFWwindow* window);

		/// @brief Llama al callback de la ventana, al cambiar el tamaño de la ventana.
		/// @param window Ventana GLFW.
		/// @param sizex Resolución horizontal en píxeles.
		/// @param sizey Resolución vertical en píxeles.
		static void GlfwResizeCallback(GLFWwindow* window, int sizex, int sizey);

		/// @brief Callback que se llama cuando se cambia de tamaño la ventana.
		/// @param sizex Resolución horizontal en píxeles.
		/// @param sizey Resolución vertical en píxeles.
		void ResizeCallback(int sizex, int sizey);

		
		// ---------------- GLFW --------------------- //

		struct GlfwWindowDeleter {
			void operator()(GLFWwindow*) const noexcept {} // NO-OP.
		};

		struct GlfwMonitorDeleter {
			void operator()(GLFWmonitor*) const noexcept {} // NO-OP.
		};

		UniquePtr<GLFWwindow, GlfwWindowDeleter> window;
		UniquePtr<GLFWmonitor, GlfwMonitorDeleter> monitor; // Para pantalla completa.

		const GLFWvidmode* monitorInfo = nullptr;

	};

}
