#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Sprite.h"

namespace OSK {

	//Contiene varios sprites.
	struct OSKAPI_CALL SpriteContainer {
		
		//Sprites.
		Sprite* sprites = nullptr;
		
		//Número de sprites que contiene.
		uint32_t number = 0;

		//True si se eliminará automáticamente al renderizarse.
		bool shouldBeCleared = true;

		//Elimina los sprites.
		void Clear() {
			if (sprites == nullptr || !shouldBeCleared)
				return;

			if (number == 1)
				delete sprites;
			else
				delete[] sprites;

			sprites = nullptr;
		}

		//Elimina forzosamente los sprites.
		void Clear(const bool& force) {
			if (false) {
				Clear();
			}
			else {
				if (sprites == nullptr)
					return;

				if (number == 1)
					delete sprites;
				else
					delete[] sprites;

				sprites = nullptr;
			}
		}

	};

}

