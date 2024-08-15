#include "IComponentView.h"

#include "OSKengine.h"
#include "AssetManager.h"

#include "EditorUiConstants.h"
#include "UiTextView.h"

#include "SdfDrawCall2D.h"

OSK::Editor::Views::IComponentView::IComponentView(const Vector2f& size, const std::string& title) : VerticalContainer(size), m_titleView(new OSK::UI::TextView(size)) {
	SetKeepRelativeSize(true);
	
	auto font = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font1.json");

	m_titleView->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	m_titleView->SetFont(font);
	m_titleView->SetFontSize(Editor::UI::Constants::MainFontSize);
	m_titleView->SetMargin({
		m_titleView->GetMarging().x,
		m_titleView->GetMarging().y,
		m_titleView->GetMarging().z,
		m_titleView->GetMarging().w + 10.0f
		});

	m_titleView->SetText(title);
	m_titleView->AdjustSizeToText();

	AddChild("title", m_titleView);
	
	GRAPHICS::SdfDrawCall2D background{};
	background.borderSize = 5.0f;
	background.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
	background.fill = true;
	background.mainColor = Editor::UI::Constants::BackgroundAlternativeColor;
	background.shape = GRAPHICS::SdfShape2D::RECTANGLE;
	background.transform.SetPosition(GetContentTopLeftPosition());
	background.transform.SetScale(GetSize());

	m_backgroundDrawCallIndex = GetAllDrawCalls().GetSize();

	AddDrawCall(background);
}

void OSK::Editor::Views::IComponentView::OnSizeChanged(const Vector2f& previousSize) {
	auto& backgroundDrawCall = GetAllDrawCalls().At(m_backgroundDrawCallIndex);
	backgroundDrawCall.transform.SetPosition(GetContentTopLeftPosition());
	backgroundDrawCall.transform.SetScale(GetSize());
}
