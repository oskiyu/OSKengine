#include "ISystemView.h"

#include "EditorUiConstants.h"
#include "UiTextView.h"
#include "OSKengine.h"
#include "Font.h"
#include "EditorUiConstants.h"

#include "UiButton.h"

OSK::Editor::Views::
ISystemView::ISystemView(const Vector2f& size, const std::string& title) : VerticalContainer({ 120.0f, 26.0f }) {
	SetAnchor(OSK::UI::Anchor::LEFT | OSK::UI::Anchor::TOP);
	SetPadding({ 2.0f, 0.0f });
}

void OSK::Editor::Views::ISystemView::SetInitialSystemState(bool isActive) {
	
}

void OSK::Editor::Views::ISystemView::OnSizeChanged(const Vector2f& previousSize) {
	VerticalContainer::OnSizeChanged(previousSize);
	return;
	auto& backgroundDrawCall = GetAllDrawCalls().At(m_backgroundDrawCallIndex);
	backgroundDrawCall.transform.SetPosition(GetContentTopLeftPosition());
	backgroundDrawCall.transform.SetScale(GetSize());
}
