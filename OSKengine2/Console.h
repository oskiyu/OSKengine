#pragma once

#include "Logger.h"
#include "DynamicArray.hpp"
#include "Font.h"

namespace OSK::GRAPHICS {
	class SpriteRenderer;
}

namespace OSK::IO {

	class Console {

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

		DynamicArray<Entry> entries = DynamicArray<Entry>::CreateReservedArray(50);
		ASSETS::Font* fuente;
		USize32 fontSize = 20;

	};

}
