#pragma once

#include <OSKengine/IAsset.h>

#include <OSKengine/UniquePtr.hpp>
#include <OSKengine/ModelComponent3D.h>
#include <OSKengine/Collider.h>
#include <OSKengine/OwnedPtr.h>

/// @brief Contiene los assets que usa un tipo de coche en concreto.
class CarAssets : public OSK::ASSETS::IAsset {

public:

	explicit CarAssets(const std::string& assetFile);

	OSK_ASSET_TYPE_REG("CarAssets");

	OSK::ASSETS::AssetRef<OSK::ASSETS::Model3D> GetModel();
	const OSK::COLLISION::Collider& GetCollider() const;

	void _SetModel(std::string m_modelPath);
	void _SetCollider(OSK::OwnedPtr<OSK::COLLISION::Collider> collider);

private:

	std::string m_modelPath;
	OSK::UniquePtr<OSK::COLLISION::Collider> m_collider = nullptr;

};
