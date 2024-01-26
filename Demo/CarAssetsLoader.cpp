#include "CarAssetsLoader.h"

#include "CarAssets.h"

#include <OSKengine/OSKengine.h>
#include <OSKengine/AssetManager.h>
#include <OSKengine/Model3D.h>
#include <OSKengine/FileIO.h>
#include <OSKengine/ConvexVolume.h>

void CarAssetsLoader::Load(const std::string& assetFilePath, CarAssets* asset) {
	const nlohmann::json assetInfo = nlohmann::json::parse(OSK::IO::FileIO::ReadFromFile(assetFilePath));

	asset->SetName(assetInfo["name"]);

	asset->_SetModel(assetInfo["model_path"]);

	OSK::OwnedPtr<OSK::COLLISION::Collider> collider = new OSK::COLLISION::Collider;

	OSK::OwnedPtr<OSK::COLLISION::ConvexVolume> convexVolume
		= new OSK::COLLISION::ConvexVolume(
			OSK::COLLISION::ConvexVolume::CreateObb({ 0.75f * 1.5f, 1.0f, 1.0f * 2.0f }));
	convexVolume->AddOffset(OSK::Vector3f(0.0f, 1.0f, 0.0f));

	collider->SetTopLevelCollider(new OSK::COLLISION::SphereCollider(3.0f));
	collider->AddBottomLevelCollider(convexVolume.GetPointer());

	asset->_SetCollider(collider);
}
