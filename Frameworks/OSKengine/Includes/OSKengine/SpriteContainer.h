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
	class OSKAPI_CALL SpriteContainer {
		
	public:

		/// <summary>
		/// Color del sprite.
		/// </summary>
		Color color;

		/// <summary>
		/// Push constants:
		/// matriz cámara * modelo del sprite.
		/// </summary>
		PushConst2D pushConst;

		/// <summary>
		/// Buffer con los vértices del sprite.
		/// Se usa para actualizar el buffer en caso de que cambien las coordenadas de textura del sprite.
		/// </summary>
		VkBuffer vertexBuffer;

		size_t bufferOffset = 0;

		/// <summary>
		/// Memoria del buffer de los vértices.
		/// Se usa para actualizar el buffer en caso de que cambien las coordenadas de textura del sprite.
		/// </summary>
		VkDeviceMemory vertexMemory;

		/// <summary>
		/// Vertices del sprite (con sus coordenadas de textura).
		/// Se usa para actualizar el buffer en caso de que cambien las coordenadas de textura del sprite.
		/// </summary>
		Vertex vertices[4];

		/// <summary>
		/// True si hay que actualizar el buffer de vértices del sprite original.
		/// </summary>
		bool hasChanged;

		/// <summary>
		/// Referencia al material del sprite.
		/// </summary>
		OldMaterialInstance* spriteMaterial;

		/// <summary>
		/// Establece el sprite que va a 'almacenar' este container.
		/// </summary>
		/// <param name="sprite">Sprite a renderizar.</param>
		/// <param name="cameraMat">Matriz de la cámara 2D.</param>
		void Set(Sprite& sprite, glm::mat4 cameraMat);

	};

}

