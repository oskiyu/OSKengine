#include "CarSelectPanel.h"

CarSelectPanel::CarSelectPanel(const OSK::Vector2f& size, OSK::ASSETS::AssetRef<OSK::ASSETS::Texture> texture) : OSK::UI::ImageView(size) {
	SetAnchor(OSK::UI::Anchor::TOP | OSK::UI::Anchor::CENTER_X);
	SetMargin(OSK::Vector2f::Zero);
	SetPadding(OSK::Vector2f::Zero);
	SetKeepRelativeSize(true);

	GetSprite().LinkAsset(texture);
	GetSprite().SetImageView(&texture->GetTextureView2D());
}
