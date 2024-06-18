#include "EditorHeader.h"

#include "OSKengine.h"
#include "AssetManager.h"
#include "Font.h"
#include "Texture.h"

#include "UiImageView.h"
#include "UiTextView.h"

#include "EditorUiConstants.h"


OSK::Editor::UI::EditorHeader::EditorHeader(const Vector2f& size) : OSK::UI::HorizontalContainer({ size.x, 40.0f }) {
	auto editorFont = Engine::GetAssetManager()->Load<OSK::ASSETS::Font>("Resources/Assets/Fonts/font1.json");

	SetMargin(Vector2f::Zero);

	// Renderizado de la barra.
	{
		GRAPHICS::SdfDrawCall2D barDrawCall{};
		barDrawCall.contentType = GRAPHICS::SdfDrawCallContentType2D::COLOR_FLAT;
		barDrawCall.shape = GRAPHICS::SdfShape2D::RECTANGLE;
		barDrawCall.mainColor = Constants::DarkColor;
		barDrawCall.transform.SetScale(GetSize());

		m_barDrawCallIndex = GetAllDrawCalls().GetSize();

		AddDrawCall(barDrawCall);
	}

	// Icono de OSKengine.
	{
		auto iconTexture = Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/Textures/editor_icon.json");
		const auto iconView = &iconTexture->GetTextureView2D();

		auto* uiIcon = new OSK::UI::ImageView(iconTexture->GetSize().ToVector2f() * (32.0f / iconTexture->GetSize().ToVector2f().y));

		GRAPHICS::SdfDrawCall2D iconDrawCall{};
		iconDrawCall.contentType = GRAPHICS::SdfDrawCallContentType2D::TEXTURE;
		iconDrawCall.shape = GRAPHICS::SdfShape2D::RECTANGLE;
		iconDrawCall.mainColor = Color::White;
		iconDrawCall.texture = iconView;
		iconDrawCall.textureCoordinates = GRAPHICS::TextureCoordinates2D::Pixels({
			0.0f, 0.0f,
			iconView->GetSize2D().ToVector2f().x,
			iconView->GetSize2D().ToVector2f().y });
		iconDrawCall.transform.SetScale(uiIcon->GetSize());

		uiIcon->AddDrawCall(iconDrawCall);
		uiIcon->SetAnchor(OSK::UI::Anchor::FULLY_CENTERED);
		uiIcon->SetMargin(Vector2f(5.0f, 0.0f));

		AddChild("icon", uiIcon);
	}
}

void OSK::Editor::UI::EditorHeader::OnSizeChanged(const Vector2f& previousSize) {
	auto& barDrawCall = GetAllDrawCalls().At(m_barDrawCallIndex);
	barDrawCall.transform.SetScale(GetSize());
}
