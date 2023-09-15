#include "UiImageView.h"

#include "Transform2D.h"
#include "SpriteRenderer.h"

using namespace OSK;
using namespace OSK::UI;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

void ImageView::Render(SpriteRenderer* renderer, Vector2f parentPosition) const {
	if (!sprite.GetView())
		return;

	Transform2D transform(EMPTY_GAME_OBJECT);
	transform.SetPosition(GetContentTopLeftPosition() + parentPosition);
	transform.SetScale(GetContentSize());

	renderer->Draw(sprite, transform);
}
