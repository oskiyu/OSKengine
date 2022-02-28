#include "AssetManager.h"

#include "IAsset.h"

using namespace OSK;

AssetManager::~AssetManager() {
	for (auto& i : assets)
		delete i.GetPointer();
}

void AssetManager::DeleteLifetime(const std::string& lifetime) {
	if (!assetsPerLifetime.ContainsKey(lifetime))
		return;

	for (auto& i : assetsPerLifetime.Get(lifetime)) {
		assets.Remove(i);
		assetsTable.Remove(i->GetAssetFilename());

		delete i;
	}

	assetsPerLifetime.Remove(lifetime);
}
