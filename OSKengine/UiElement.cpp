#include "UiElement.h"

#include "ContentManager.h"
#include "VulkanRenderer.h"
#include "SpriteBatch.h"
#include "Font.h"

using namespace OSK;

void UiElement::SetPositionFromAnchor(const OSK::Vector2f& newPosition) {
	OSK::Vector2f diff = newPosition - positionDeltaFromAnchor;
	positionDeltaFromAnchor = newPosition;

	UpdateChildPositions(diff);
}

void UiElement::SetAnchorPosition(const OSK::Vector2f& newAnchor) {
	OSK::Vector2f diff = newAnchor - anchorPosition;
	anchorPosition = newAnchor;

	UpdateChildPositions(diff);
}

void UiElement::AddPosition(const OSK::Vector2f& diff) {
	positionDeltaFromAnchor += diff;

	UpdateChildPositions(diff);
}

void UiElement::UpdateChildPositions(const OSK::Vector2f& diff) {
	UpdateSpriteMatrix();

	for (auto child : children)
		child->AddPosition(diff);
}

void UiElement::UpdateAnchoredChildPosition(UiElement* element, bool init) {
	switch (element->anchor) {

	case UiAnchor::TOP_LEFT:
		break;

	case UiAnchor::LEFT:
	{
		element->SetAnchorPosition({ 0, size.Y / 2 });

		if (init) {
			float y = element->positionDeltaFromAnchor.Y - (element->marging.Y * 2 + element->size.Y) / 2;
			element->SetPositionFromAnchor({ element->positionDeltaFromAnchor.X, y });
		}
	}
	break;

	case UiAnchor::BOTTOM_LEFT:
	{
		element->SetAnchorPosition({ 0, size.Y });

		if (init) {
			float y = -(element->positionDeltaFromAnchor.Y + element->size.Y + element->marging.Y * 2);
			element->SetPositionFromAnchor({ element->positionDeltaFromAnchor.X, y });
		}
	}
	break;

	case UiAnchor::BOTTOM:
	{
		element->SetAnchorPosition({ size.X / 2, size.Y });

		if (init) {
			float x = element->positionDeltaFromAnchor.X - (element->marging.X * 2 + element->size.X) / 2;
			float y = -(element->positionDeltaFromAnchor.Y + element->size.Y + element->marging.Y * 2);
			element->SetPositionFromAnchor({ x, y });
		}
	}
	break;

	case UiAnchor::BOTTOM_RIGHT:
	{
		element->SetAnchorPosition({ size.X, size.Y });

		if (init) {
			float x = -(element->positionDeltaFromAnchor.X + element->size.X + element->marging.X * 2);
			float y = -(element->positionDeltaFromAnchor.Y + element->size.Y + element->marging.Y * 2);
			element->SetPositionFromAnchor({ x, y });
		}
	}
	break;
	case UiAnchor::RIGHT:
	{
		element->SetAnchorPosition({ size.X, size.Y / 2 });

		if (init) {
			float x = -(element->positionDeltaFromAnchor.X + element->size.X + element->marging.X * 2);
			float y = element->positionDeltaFromAnchor.Y - (element->marging.Y * 2 + element->size.Y) / 2;
			element->SetPositionFromAnchor({ x, y });
		}
	}
	break;

	case UiAnchor::TOP_RIGHT:
	{
		element->SetAnchorPosition({ size.X, 0 });

		if (init) {
			float x = -(element->positionDeltaFromAnchor.X + element->size.X + element->marging.X * 2);
			float y = element->positionDeltaFromAnchor.Y;
			element->SetPositionFromAnchor({ x, y });
		}
	}
	break;

	case UiAnchor::TOP:
	{
		element->SetAnchorPosition({ size.X / 2, 0 });

		if (init) {
			float x = element->positionDeltaFromAnchor.X - (element->marging.X * 2 + element->size.X) / 2;
			float y = element->positionDeltaFromAnchor.Y;
			element->SetPositionFromAnchor({ x, y });
		}
	}
	break;

	case UiAnchor::CENTER:
	{
		element->SetAnchorPosition({ size.X / 2, size.Y / 2 });

		if (init) {
			float x = element->positionDeltaFromAnchor.X - (element->marging.X * 2 + element->size.X) / 2;
			float y = element->positionDeltaFromAnchor.Y - (element->marging.Y * 2 + element->size.Y) / 2;
			element->SetPositionFromAnchor({ x, y });
		}
	}
	break;

	}
}

void UiElement::Draw(OSK::SpriteBatch* spriteBatch) {
	if (sprite.texture)
		spriteBatch->DrawSprite(sprite);

	if (fuente)
		spriteBatch->DrawString(fuente, text, 1.0f, GetPosition() + marging, textColor);

	for (auto child : children)
		if (child->isActive)
			child->Draw(spriteBatch);
}

void UiElement::AddElement(UiElement* element) {
	element->parent = this;

	if (layout == UiLayout::VERTICAL || layout == UiLayout::HORIZONTAL) {
		element->SetPositionFromAnchor(GetPosition() + nextPos + padding + element->marging);
		children.push_back(element);

		if (layout == UiLayout::VERTICAL) {
			nextPos.Y += element->size.Y + element->marging.Y * 2;

			if (size.Y < nextPos.Y)
				size.Y = nextPos.Y;
		}
		else if (layout == UiLayout::HORIZONTAL) {
			nextPos.X += element->size.X + element->marging.X * 2;

			if (size.X < nextPos.X)
				size.X = nextPos.X;
		}

		Shrink();
	}
	else if (layout == UiLayout::FIXED) {
		UpdateAnchoredChildPosition(element, true);

		children.push_back(element);
	}
}

void UiElement::Shrink() {
	float x = 0.0f;
	float y = 0.0f;
	if (shrinkToChildren) {
		if (layout == UiLayout::VERTICAL) {
			for (auto child : children)
				if (child->isActive && child->size.X > x)
					x = child->size.X + child->marging.X * 2;

			for (auto child : children)
				if (child->isActive && child->size.Y)
					y += child->size.Y + child->marging.Y * 2;
		}

		if (layout == UiLayout::HORIZONTAL) {
			for (auto child : children)
				if (child->isActive && child->size.X)
					x += child->size.X + child->marging.X * 2;

			for (auto child : children)
				if (child->isActive && child->size.Y > y)
					y = child->size.Y + child->marging.Y * 2;
		}
	}

	size = OSK::Vector2f(x, y) + padding * 2;

	UpdateSpriteMatrix();
}

void UiElement::Update(const Vector4f& mouserRec, bool click) {
	if (mouserRec.Intersects({ GetPosition().X, GetPosition().Y, size.X, size.Y })) {
		if (spriteColorOnHover.has_value())
			sprite.color = spriteColorOnHover.value();

		if (click) {
			if (!previouslyOnClick)
				onClick(this, mouserRec.GetRectanglePosition());

			onHold(this, mouserRec.GetRectanglePosition());
		}
	}
	else {
		sprite.color = originalSpireColor;
	}

	previouslyOnClick = click;

	for (auto child : children)
		if (child->isActive)
			child->Update(mouserRec, click);
}

void UiElement::InitSprite(OSK::ContentManager* content, OSK::RenderAPI* renderer) {
	content->CreateSprite(&sprite);
	sprite.material = renderer->GetMaterialSystem()->GetMaterial(MPIPE_2D)->CreateInstance().GetPointer();
	sprite.UpdateMaterialTexture();
	sprite.material->FlushUpdate();

	UpdateSpriteMatrix();
}

void UiElement::UpdateSpriteMatrix() {
	sprite.transform.SetPosition(GetPosition());
	sprite.transform.SetScale(size);
}


OSK::Vector2f UiElement::GetPosition() const {
	return anchorPosition + positionDeltaFromAnchor;
}

OSK::Vector2f UiElement::GetSize() const {
	return size;
}

void UiElement::SetSize(const OSK::Vector2f& size) {
	this->size = size;

	UpdateSpriteMatrix();

	if (layout == UiLayout::FIXED)
		for (auto child : children)
			UpdateAnchoredChildPosition(child);
}