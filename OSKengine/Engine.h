#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "WindowAPI.h"
#include "VulkanRenderer.h"
#include "AudioAPI.h"

namespace OSK {

	class OSKAPI_CALL Engine {

	public:

		void SetupWindow();
		void SetupRenderer();

		void Run();

		RenderAPI* GetRenderer();
		WindowAPI* GetWindow();
		AudioSystem* GetAudioSystem();

	private:

		RenderAPI* renderer = nullptr;
		WindowAPI* window = nullptr;
		AudioSystem* audio = nullptr;

		KeyboardState OldKS{};
		KeyboardState NewKS{};

		MouseState OldMS{};
		MouseState NewMS{};

	};

}