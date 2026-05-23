#pragma once

#include "Platforms.h"
#ifdef OSK_ANDROID

#include "ApiCall.h"
#include "IDisplay.h"

struct ANativeWindow;
struct android_app;

namespace OSK::IO {

	/// @brief Representa la ventana de renderizado
	/// del dispositivo Android.
	/// 
	/// @note Al crearla, su parámetro de resolución
	/// se ignora, y tendrá siempre la resolución
	/// del móvil.
	class OSKAPI_CALL AndroidWindow : public IDisplay {

	public:

		/// @param app Aplicación Android nativa.
		explicit AndroidWindow(android_app* app);
		~AndroidWindow() override = default;

		/// @note Ignora ambos parámetros.
		void Create(Vector2ui _, const std::string& title) override;
		void Update() override;
		void Close() override;

		void QueryInterface(TInterfaceUuid interfaceUuid, void** ptr) const override;
		void QueryConstInterface(TInterfaceUuid uuid, const void** ptr) const override;

		ANativeWindow* GetNativeWindow();
		const ANativeWindow* GetNativeWindow() const;

	private:

		android_app* m_app = nullptr;

	};

}

#endif // OSK_ANDROID
