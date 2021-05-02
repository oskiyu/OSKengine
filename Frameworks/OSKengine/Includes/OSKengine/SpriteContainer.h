#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Sprite.h"

namespace OSK {

	/// <summary>
	/// Clase que almacena los componentes necesarios para renderizar un sprite.
	/// Se usa internamente dentro del SpriteBatch.
	/// </summary>
	struct OSKAPI_CALL SpriteContainer {
		
		/// <summary>
		/// Color del sprite.
		/// </summary>
		Color SpriteColor;

		/// <summary>
		/// Push constants:
		/// matriz c�mara * modelo del sprite.
		/// </summary>
		PushConst2D PConst;

		/// <summary>
		/// Buffer con los v�rtices del sprite.
		/// Se usa para actualizar el buffer en caso de que cambien las coordenadas de textura del sprite.
		/// </summary>
		VkBuffer VertexBuffer;

		/// <summary>
		/// Memoria del buffer de los v�rtices.
		/// Se usa para actualizar el buffer en caso de que cambien las coordenadas de textura del sprite.
		/// </summary>
		VkDeviceMemory VertexMemory;

		/// <summary>
		/// Vertices del sprite (con sus coordenadas de textura).
		/// Se usa para actualizar el buffer en caso de que cambien las coordenadas de textura del sprite.
		/// </summary>
		Vertex Vertices[4];

		/// <summary>
		/// True si hay que actualizar el buffer de v�rtices del sprite original.
		/// </summary>
		bool hasChanged;

		/// <summary>
		/// Referencia al material del sprite.
		/// </summary>
		MaterialInstance* SpriteMaterial;

		/// <summary>
		/// Establece el sprite que va a 'almacenar' este container.
		/// </summary>
		/// <param name="sprite">Sprite a renderizar.</param>
		/// <param name="cameraMat">Matriz de la c�mara 2D.</param>
		void Set(Sprite& sprite, glm::mat4 cameraMat);

	};

}

