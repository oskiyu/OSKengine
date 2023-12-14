#include "CarAssets.h"

#include <OSKengine/OSKengine.h>
#include <OSKengine/AssetManager.h>

CarAssets::CarAssets(const std::string& assetFile) : OSK::ASSETS::IAsset(assetFile) {

}

OSK::ASSETS::AssetRef<OSK::ASSETS::Model3D> CarAssets::GetModel() {
	return OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Model3D>(m_modelPath);
}

const OSK::COLLISION::Collider& CarAssets::GetCollider() const {
	return m_collider.GetValue();
}

void CarAssets::_SetModel(std::string modelPath) {
	m_modelPath = modelPath;
}

void CarAssets::_SetCollider(OSK::OwnedPtr<OSK::COLLISION::Collider> collider) {
	m_collider = collider.GetPointer();
}
