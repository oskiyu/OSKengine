#include "IComponentView.h"

#include "OSKengine.h"
#include "AssetManager.h"

#include "EditorUiConstants.h"
#include "UiTextView.h"

#include "SdfDrawCall2D.h"

using namespace OSK;
using namespace OSK::ASSETS;

OSK::Editor::Views::IComponentView::IComponentView(const Vector2f& size, const std::string& title) : 
	CollapsibleWrapper(
		new VerticalContainer({ 120.0f, 26.0f }),
		title,
		Engine::GetAssetManager()->Load<Font>(Editor::UI::Constants::EditorFontPath),
		Editor::UI::Constants::MainFontSize), m_titleView(new OSK::UI::TextView(size)) 
{
	GetUnderlyingContainer()->SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);

	GRAPHICS::SdfDrawCall2D background{};
	background.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
	background.fill = true;
	background.mainColor = Editor::UI::Constants::BackgroundAlternativeColor;
	background.shape = GRAPHICS::SdfShape2D::RECTANGLE;
	background.transform.SetPosition(GetContentTopLeftPosition());
	background.transform.SetScale(GetSize());

	// GetAllDrawCalls().GetSize() < UIndex32::max.
	m_backgroundDrawCallIndex = static_cast<UIndex32>(GetAllDrawCalls().GetSize());

	// Botón.
	{
		GRAPHICS::SdfDrawCall2D background{};
		background.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
		background.fill = true;
		background.shape = GRAPHICS::SdfShape2D::RECTANGLE;
		background.transform.SetPosition(GetContentTopLeftPosition());
		background.transform.SetScale(GetSize());

		background.mainColor = Editor::UI::Constants::DarkColor;
		GetToggleBar()->GetDefaultDrawCalls().Insert(background);

		background.mainColor = Editor::UI::Constants::HoveredColor;
		GetToggleBar()->GetSelectedDrawCalls().Insert(background);

		background.mainColor = Editor::UI::Constants::DarkColor;
		GetToggleBar()->GetPressedDrawCalls().Insert(background);
	}

	AddDrawCall(background);
}

void OSK::Editor::Views::IComponentView::OnSizeChanged(const Vector2f& previousSize) {
	CollapsibleWrapper::OnSizeChanged(previousSize);

	auto& backgroundDrawCall = GetAllDrawCalls().At(m_backgroundDrawCallIndex);
	backgroundDrawCall.transform.SetPosition(GetContentTopLeftPosition());
	backgroundDrawCall.transform.SetScale(GetSize());
}

OSK::UI::VerticalContainer* OSK::Editor::Views::IComponentView::GetUnderlyingContainer() {
	return GetContent()->As<VerticalContainer>();
}
