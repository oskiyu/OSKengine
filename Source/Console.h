#pragma once

#include "Logger.h"
#include "DynamicArray.hpp"
#include "Font.h"

namespace OSK::GRAPHICS {
	class SpriteRenderer;
}

namespace OSK::IO {

	class OSKAPI_CALL Console {

	public:

		struct Entry {

			std::string text;
			TDeltaTime timestamp = 0.0f;

		};

	public:

		void SetFont(ASSETS::Font* font);

		void WriteLine(const std::string& msg);
		void Draw(GRAPHICS::SpriteRenderer* spriteRenderer);

	private:

		DynamicArray<Entry> m_entries = DynamicArray<Entry>::CreateReservedArray(50);
		ASSETS::Font* m_fuente = nullptr;
		USize32 m_fontSize = 23;

	};

}
