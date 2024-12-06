#include "EditorObjectList.h"

#include "OSKengine.h"
#include "AssetManager.h"

#include "EditorObjectPropertiesPanel.h"

#include "EditorUiConstants.h"
#include "EditorPanelTitle.h"

#include "Editor.h"


OSK::Editor::UI::ObjectList::ObjectList(const Vector2f& size, OSK::Editor::Editor* editor) : OSK::UI::VerticalContainer(size), m_editorRef(editor) {
	const Vector2f textSize = { size.x - 20.0f, 25.0f };

	SetMargin(Vector2f::Zero);
	SetPadding(Vector2f::Zero);

	{
		GRAPHICS::SdfDrawCall2D backgroundDrawCall{};
		backgroundDrawCall.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
		backgroundDrawCall.shape = GRAPHICS::SdfShape2D::RECTANGLE;
		backgroundDrawCall.mainColor = Constants::BackgroundColor;
		backgroundDrawCall.transform.SetScale(GetSize());

		m_backgroundDrawCallIndex = GetAllDrawCalls().GetSize();

		AddDrawCall(backgroundDrawCall);
	}

	m_title = new EditorPanelTitle(textSize);
	m_title->SetText("Objetos");

	AddChild("title", m_title);

	AddCreateObjectButton();

	auto font = Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font1.json");

	for (UIndex64 i = 0; i < MaxShownObjects; i++) {
		auto* view = new OSK::UI::Button(textSize);
		view->SetPadding(Vector2f(5.0f));
		view->SetMargin(Vector4f(5.0f, 1.0f, 5.0f, 1.0f));

		view->SetTextFont(font);
		view->SetTextFontSize(Constants::MainFontSize);

		{
			GRAPHICS::SdfDrawCall2D selectedDrawCall{};
			selectedDrawCall.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
			selectedDrawCall.shape = GRAPHICS::SdfShape2D::RECTANGLE;
			selectedDrawCall.mainColor = Constants::HoveredColor;
			selectedDrawCall.transform.SetScale(view->GetSize());

			view->GetSelectedDrawCalls().Insert(selectedDrawCall);
		}

		{
			GRAPHICS::SdfDrawCall2D pressedDrawCall{};
			pressedDrawCall.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
			pressedDrawCall.shape = GRAPHICS::SdfShape2D::RECTANGLE;
			pressedDrawCall.mainColor = Constants::SelectedColor;
			pressedDrawCall.transform.SetScale(view->GetSize());

			view->GetPressedDrawCalls().Insert(pressedDrawCall);
		}

		if (i % 2 == 0) {
			GRAPHICS::SdfDrawCall2D defaultDrawCall{};
			defaultDrawCall.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
			defaultDrawCall.shape = GRAPHICS::SdfShape2D::RECTANGLE;
			defaultDrawCall.mainColor = Constants::BackgroundAlternativeColor;
			defaultDrawCall.transform.SetScale(view->GetSize());

			view->GetDefaultDrawCalls().Insert(defaultDrawCall);
		}

		AddChild(std::to_string(i), view);
		m_textViews.Insert(view);
	}

	for (auto& view : m_textViews) {
		view->SetCallback([&](bool isPressed) {
			if (isPressed) {
				this->ClearSelection();
				view->SetState(OSK::UI::Button::State::PRESSED);
				m_propertiesPanel->ShowContent();
				m_propertiesPanel->SetSubtitle(static_cast<std::string>(view->GetText()));

				auto text = static_cast<std::string>(view->GetText().substr(view->GetText().find_first_of("0123456789")));
				auto obj = std::stoull(text);
				// m_propertiesPanel->UpdateByObject(OSK::ECS::GameObjectIndex(obj));
				m_editorRef->SetSelectedObject(OSK::ECS::GameObjectIndex(obj));
				m_selectedObject = OSK::ECS::GameObjectIndex(obj);
				m_propertiesPanel->SetObject(OSK::ECS::GameObjectIndex(obj));
			}
			else {
				m_propertiesPanel->ClearContent();
				m_editorRef->ClearSelectedObject();
			}
			});
	}

	m_propertiesPanel = new ObjectPropertiesPanel({ size.x, 300.0f });
	m_propertiesPanel->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::CENTER_Y);
	AddChild(ObjectPropertiesPanel::Name, m_propertiesPanel);
}

void OSK::Editor::UI::ObjectList::OnSizeChanged(const Vector2f&) {
	auto& backgroundDrawCall = GetAllDrawCalls()[m_backgroundDrawCallIndex];
	backgroundDrawCall.transform.SetScale(GetSize());
}

void OSK::Editor::UI::ObjectList::ClearObjects() {
	for (OSK::UI::Button* view : m_textViews) {
		view->SetInvisible();
	}
}

void OSK::Editor::UI::ObjectList::SetObjects(std::span<const OSK::ECS::GameObjectIndex> objects) {
	for (UIndex64 i = 0; i < std::min(objects.size(), m_textViews.GetSize()); i++) {
		m_textViews[i]->SetVisible();
		m_textViews[i]->SetText(std::format("Objeto {}", objects[i].Get()));
	}

	Rebuild();
}

void OSK::Editor::UI::ObjectList::ClearSelection() {
	for (auto* view : m_textViews) {
		if (!view->IsVisible()) {
			break;
		}

		view->SetState(OSK::UI::Button::State::DEFAULT);
	}
}

OSK::ECS::GameObjectIndex OSK::Editor::UI::ObjectList::GetSelectedObject() const {
	return m_selectedObject;
}

void OSK::Editor::UI::ObjectList::AddCreateObjectButton() {
	auto newObjectButton = new OSK::UI::Button({ GetSize().x - 20.0f, 25.0f}, "Nuevo objeto");

	newObjectButton->SetMargin({
		newObjectButton->GetMarging2D().x,
		newObjectButton->GetMarging2D().y + 3.0f
		});

	newObjectButton->SetType(OSK::UI::Button::Type::NORMAL);
	newObjectButton->SetTextFont(Engine::GetAssetManager()->Load<OSK::ASSETS::Font>(OSK::Editor::UI::Constants::EditorFontPath));
	newObjectButton->SetTextFontSize(OSK::Editor::UI::Constants::SecondaryFontSize);
	newObjectButton->SetCallback([this](bool) {
		Engine::GetEcs()->SpawnObject();
		});

	{
		GRAPHICS::SdfDrawCall2D background{};
		background.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
		background.fill = true;
		background.shape = GRAPHICS::SdfShape2D::RECTANGLE;
		background.transform.SetPosition(GetContentTopLeftPosition());
		background.transform.SetScale(newObjectButton->GetSize());

		background.mainColor = OSK::Editor::UI::Constants::DefaultGreenColor;
		newObjectButton->GetDefaultDrawCalls().Insert(background);

		background.mainColor = OSK::Editor::UI::Constants::HoveredGreenColor;
		newObjectButton->GetSelectedDrawCalls().Insert(background);

		background.mainColor = OSK::Editor::UI::Constants::SelectedGreenColor;
		newObjectButton->GetPressedDrawCalls().Insert(background);
	}

	AddChild("newObjectButton", newObjectButton);
}

void OSK::Editor::UI::ObjectList::ClearAllComponentViews() {
	m_propertiesPanel->ClearAllComponentViews();
}

void OSK::Editor::UI::ObjectList::AddComponentView(OwnedPtr<OSK::Editor::Views::IComponentView> view) {
	m_propertiesPanel->AddComponentView(view);
}
