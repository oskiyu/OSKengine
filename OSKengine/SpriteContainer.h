#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Sprite.h"
#include "Camera2D.h"

namespace OSK {

	/// <summary>
	/// Clase que almacena los componentes necesarios para renderizar un sprite.
	/// Se usa internamente dentro del SpriteBatch.
	/// </summary>
	class OSKAPI_CALL SpriteContainer {
		
	public:

		SpriteContainer(Camera2D* camera) : camera(camera) {

		}

		Camera2D* camera = nullptr;
		std::vector<Sprite> spritesToDraw;
		
	};

}
