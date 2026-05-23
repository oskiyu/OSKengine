#pragma once

#include "Platforms.h"
#ifdef OSK_ANDROID

#include "ApiCall.h"

#include "IUserInput.h"
#include "ITouchInput.h"
#include "TouchInput.h"


struct android_app;
struct AInputEvent;

namespace OSK::IO {

	/// @brief Implementa los métodos de entrada disponibles
	/// en Android.
	/// 
	/// Implementa las interfaces:
	/// - ITouchInput.
	/// 
	/// @todo Sincornización entre hilos (?).
	class OSKAPI_CALL AndroidInput : public IUserInput, public ITouchInput {

	public:

		/// @param app Aplicación Android nativa.
		AndroidInput(android_app* app);
		~AndroidInput() override = default;

		void Update() override;

		void QueryInterface(TInterfaceUuid uuid, void** ptr) const override;
		void QueryConstInterface(TInterfaceUuid uuid, const void** ptr) const override;

	private:

		/// @brief Callback que se llamará cada vez que se registre un toque 
		/// en la pantalla.
		/// 
		/// Guardará el evento en `m_inputCache`.
		/// @param app Aplicación Android nativa.
		/// @param event Evento de entrada.
		/// @return 1 si ha procesado el input, 0 si no.
		static int32_t HandleInputEvent(android_app* app, AInputEvent* event);


		/// @brief Almacena los eventos que se van generando a lo
		/// largo del frame.
		/// 
		/// Después de terminar el frame, su contenido se copiará
		/// a `m_currentFrameInputs`.
		DynamicArray<TouchInput> m_inputCache{};

		/// @brief Referencia a la aplicación Android nativa.
		android_app* m_app = nullptr;

		static AndroidInput* m_self;

	};

}

#endif // OSK_ANDROID
