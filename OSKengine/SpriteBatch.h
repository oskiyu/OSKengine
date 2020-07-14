#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "DrawCommand.h"
#include "Texture.h"
#include "Sprite.h"
#include "Font.h"
#include "TextRenderingLimitActionsEnum.h"
#include "AnchorEnum.h"
#include "AnchorTextToEnum.h"

namespace OSK {

	class OSKAPI_CALL SpriteBatch {
		friend class VulkanRenderer;
	public:
		void DrawSprite(Sprite sprite);

		void DrawString(const Font& fuente, const std::string& texto, const float_t& size, const Vector2& position, const Color& color = Color(1.0f), const Anchor& screenAnchor = Anchor::TOP_LEFT, const Vector4& reference = Vector4(-1.0f), const TextRenderingLimit& limitAction = TextRenderingLimit::DO_NOTHING, const float_t& sizeXlimit = 0, const float_t& limitOffset = 10);

		void Clear();

	private:
		
		SpriteBatch();

		Vector2 GetTextPosition(const Vector2& position, const Vector2& textSize, const Anchor& anchor, const AnchorTextTo& to, const Vector4& reference = Vector4(0.0f)) const;

		VulkanRenderer* renderer = nullptr;

		std::vector<Sprite> spritesToDraw{};

	};

}
