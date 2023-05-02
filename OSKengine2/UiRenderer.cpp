#include "UiRenderer.h"

#include "UiElement.h"
#include "ICommandList.h"
#include "Sprite.h"
#include "Transform2D.h"

using namespace OSK;
using namespace OSK::UI;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

void UiRenderer::RenderElement(const UiElement* element, const Transform2D &transform) {
	/*spriteRenderer.Draw(element->sprite, transform);

	for (auto& i : element->GetChildElements()) {
		Transform2D newTransform = transform;
		newTransform.AddPosition(i->GetPosition());
		newTransform.AddScale(i->GetSize());

		RenderElement(i.GetPointer(), newTransform);
	}*/
}

void UiRenderer::Render(ICommandList* commandList, const UiElement* element) {
	/*spriteRenderer.SetCommandList(commandList);
	spriteRenderer.Begin();

	Transform2D transform(0);
	transform.SetPosition(element->GetPosition());
	transform.SetScale(element->GetSize());

	RenderElement(element, transform);

	spriteRenderer.End();*/
}
