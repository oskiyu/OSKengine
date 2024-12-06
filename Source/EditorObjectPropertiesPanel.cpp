#include "EditorObjectPropertiesPanel.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "AssetManager.h"
#include "EditorUiConstants.h"
#include "EditorObjectList.h"

#include "Font.h"

OSK::Editor::UI::ObjectPropertiesPanel::ObjectPropertiesPanel(const Vector2f& size) : PropertiesPanel(size) {
	auto deleteObjectButton = new OSK::UI::Button({ GetSize().x - 20.0f, 25.0f }, "Eliminar objeto");

	deleteObjectButton->SetMargin({
		deleteObjectButton->GetMarging2D().x,
		deleteObjectButton->GetMarging2D().y + 0.0f
		});
	deleteObjectButton->SetType(OSK::UI::Button::Type::NORMAL);
	deleteObjectButton->SetTextFont(Engine::GetAssetManager()->Load<OSK::ASSETS::Font>(OSK::Editor::UI::Constants::EditorFontPath));
	deleteObjectButton->SetTextFontSize(OSK::Editor::UI::Constants::SecondaryFontSize);
	deleteObjectButton->SetCallback([this](bool) {
		auto obj = GetParent()->As<ObjectList>()->GetSelectedObject();
		if (!obj.IsEmpty()) {
			Engine::GetEcs()->DestroyObject(&obj);
			GetParent()->As<ObjectList>()->ClearSelection();
		}
		});

	{
		GRAPHICS::SdfDrawCall2D background{};
		background.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
		background.fill = true;
		background.shape = GRAPHICS::SdfShape2D::RECTANGLE;
		background.transform.SetPosition(GetContentTopLeftPosition());
		background.transform.SetScale(deleteObjectButton->GetSize());

		background.mainColor = OSK::Editor::UI::Constants::DefaultRedColor;
		deleteObjectButton->GetDefaultDrawCalls().Insert(background);

		background.mainColor = OSK::Editor::UI::Constants::HoveredRedColor;
		deleteObjectButton->GetSelectedDrawCalls().Insert(background);

		background.mainColor = OSK::Editor::UI::Constants::SelectedRedColor;
		deleteObjectButton->GetPressedDrawCalls().Insert(background);
	}

	AddChild("deleteObjectButton", deleteObjectButton);
}

void OSK::Editor::UI::ObjectPropertiesPanel::SetObject(const OSK::ECS::GameObjectIndex obj) {
	GetChild("deleteObjectButton")->SetVisible();
}

void OSK::Editor::UI::ObjectPropertiesPanel::AddComponentView(OwnedPtr<OSK::Editor::Views::IComponentView> view) {
	AddChild(std::format("content{}", m_nextViewIndex), view.GetPointer());
	m_nextViewIndex++;
	GetChild("deleteObjectButton")->SetVisible();
}

void OSK::Editor::UI::ObjectPropertiesPanel::ClearAllComponentViews() {
	for (UIndex32 i = 0; i < m_nextViewIndex; i++) {
		auto key = std::format("content{}", i);
		DeleteChild(key);
	}

	m_nextViewIndex = 0;
}

void OSK::Editor::UI::ObjectPropertiesPanel::EmplaceChild(IElement* child) {
	if (child->Is<OSK::Editor::Views::IComponentView>()) {
		child->SetMargin(Vector2f(8.0f, 2.0f));
		child->SetPadding(Vector2f(8.0f, 8.0f));
		child->As<OSK::Editor::Views::IComponentView>()->AdjustSizeToChildren();
		child->SetSize({
			GetContentSize().x,
			child->GetContentSize().y
			});

		child->As<OSK::Editor::Views::IComponentView>()->GetToggleBar()->SetSize({
			GetContentSize().x - child->GetMarging2D().x,
			child->As<OSK::Editor::Views::IComponentView>()->GetToggleBar()->GetSize().y
			});
	}

	VerticalContainer::EmplaceChild(child);
}

void OSK::Editor::UI::ObjectPropertiesPanel::ClearContent() {
	PropertiesPanel::ClearContent();
	ClearAllComponentViews();
}
