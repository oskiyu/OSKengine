#include "AssetManager.h"

#include "IAsset.h"

using namespace OSK;

AssetManager::~AssetManager() {
	
}

void AssetManager::DeleteLifetime(const std::string& lifetime) {
	if (!assetsPerLifetime.ContainsKey(lifetime))
		return;

	for (auto& i : assetsPerLifetime.Get(lifetime))
		assetsTable.Remove(i->GetAssetFilename());

	assetsPerLifetime.Remove(lifetime);
}