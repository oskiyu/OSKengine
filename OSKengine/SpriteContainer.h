#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Sprite.h"

namespace OSK {

	//Contiene un sprite.
	struct OSKAPI_CALL SpriteContainer {
		
		Color SpriteColor;
		PushConst2D PConst;
		VkBuffer VertexBuffer;
		VkDeviceMemory VertexMemory;

		Vertex Vertices[4];
		bool hasChanged;

		MaterialInstance* SpriteMaterial;

		void Set(Sprite& sprite, glm::mat4 cameraMat) {
			SpriteMaterial = sprite.SpriteMaterial.GetPointer();
			SpriteColor = sprite.color;
			PConst = sprite.getPushConst(cameraMat);
			VertexBuffer = sprite.VertexBuffer.Buffer;
			VertexMemory = sprite.VertexBuffer.Memory;
			hasChanged = sprite.hasChanged;

			for (uint32_t i = 0; i < 4; i++)
				Vertices[i] = sprite.Vertices[i];
		}

	};

}

