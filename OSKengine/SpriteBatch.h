#pragma once

#include <vulkan/vulkan.h>
#include <queue>
#include "DrawCommand.h"
#include "Texture.h"
#include "Sprite.h"
#include "Font.h"
#include "TextRenderingLimitActionsEnum.h"
#include "AnchorEnum.h"
#include "AnchorTextToEnum.h"

namespace OSK {

	//Clase para el renderizado 2D.
	class OSKAPI_CALL SpriteBatch {
		friend class VulkanRenderer;
	public:

		//Dibuja un sprite.
		void DrawSprite(Sprite sprite);

		//Dibuja un texto.
		//	<fuente>: fuente del texto.
		//	<texto>: texto que se renderiza.
		//	<position>: posición en la que se renderiza el texto, en píxeles desde el ancla (<screenAnchor>), por defecto desde la esquina inferior izquierda de la pantalla.
		//	<color>: color del texto.
		//	<screenAnchor>: establece el origen de coordenadas desde el que se calcula la posición del texto.
		//	<limitAction>: establece que ocurre cuando el texto sale de la pantalla.
		//	<sizeXlimit>: si > 0, establece, en píxeles, el límite sobre el cual se aplica <limitAction>.
		void DrawString(const Font* fuente, const std::string& texto, const float_t& size, const Vector2& position, const Color& color = Color(1.0f), const Anchor& screenAnchor = Anchor::TOP_LEFT, const Vector4& reference = Vector4(-1.0f), const TextRenderingLimit& limitAction = TextRenderingLimit::DO_NOTHING, const float_t& sizeXlimit = 0, const float_t& limitOffset = 10);

		//Vacía el spriteBatch.
		void Clear();

	private:
		
		SpriteBatch();

		Vector2 GetTextPosition(const Vector2& position, const Vector2& textSize, const Anchor& anchor, const AnchorTextTo& to, const Vector4& reference = Vector4(0.0f)) const;

		VulkanRenderer* renderer = nullptr;

		//2.7
		//170
		//vs
		//2.5
		//190
		std::deque<Sprite> spritesToDraw{};

	};

}
