#include "IUserInterfaceHasChildren.h"

namespace OSK::UI {

	IUserInterfaceHasChildren::IUserInterfaceHasChildren(BaseUIElement* base) {
		this->base = base;
	}


	void IUserInterfaceHasChildren::Draw(const RenderAPI& renderer) {
		if (base == nullptr)
			return;

		for (auto i : Children) {
			if (i != nullptr && i->IsActive && i->IsVisible)
				i->Draw(renderer);
		}
	}


	void IUserInterfaceHasChildren::AddElement(BaseUIElement* element) {
		if (base == nullptr)
			return;
		
		for (auto i : Children) {
			if (i == element)
				return;
		}

		if (element->FillParent)
			element->SetRectangle(base->GetRectangle());

		Children.push_back(element);
		element->Parent = base;

		element->SetPositionRelativeTo(base);
	}


	void IUserInterfaceHasChildren::RemoveElement(BaseUIElement* element) {
		if (base == nullptr)
			return;

		for (uint32_t i = 0; i = Children.size(); i++) {
			if (Children[i] == element) {
				Children[i]->Parent = nullptr;
				Children.erase(Children.begin() + i);

				break;
			}
		}
	}


	void IUserInterfaceHasChildren::UpdateChildrenPositions() {
		if (base == nullptr)
			return;

		for (auto i : Children) {
			if (i != nullptr)
				i->SetPositionRelativeTo(base);
		}
	}

}