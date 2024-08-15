#include "EditorObjectPropertiesPanel.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "AssetManager.h"
#include "EditorUiConstants.h"

OSK::Editor::UI::ObjectPropertiesPanel::ObjectPropertiesPanel(const Vector2f& size) : PropertiesPanel(size) {

}

void OSK::Editor::UI::ObjectPropertiesPanel::SetObject(const OSK::ECS::GameObjectIndex obj) {
	(void)0;
}

void OSK::Editor::UI::ObjectPropertiesPanel::AddComponentView(OwnedPtr<OSK::Editor::Views::IComponentView> view) {
	AddChild(std::format("content{}", m_nextViewIndex), view.GetPointer());
	m_nextViewIndex++;
}

void OSK::Editor::UI::ObjectPropertiesPanel::ClearAllComponentViews() {
	for (UIndex32 i = 0; i < m_nextViewIndex; i++) {
		auto key = std::format("content{}", i);
		DeleteChild(key);
	}

	m_nextViewIndex = 0;
}

void OSK::Editor::UI::ObjectPropertiesPanel::EmplaceChild(IElement* child) {
	if (child->Is<Editor::Views::IComponentView>()) {
		child->SetMargin(Vector2f(8.0f, 2.0f));
		child->SetPadding(Vector2f(12.0f, 12.0f));
		child->As<Editor::Views::IComponentView>()->AdjustSizeToChildren();
		child->SetSize({
			GetContentSize().x,
			child->GetContentSize().y
			});
	}

	VerticalContainer::EmplaceChild(child);
}
