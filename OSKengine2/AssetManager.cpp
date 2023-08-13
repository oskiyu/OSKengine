#include "AssetManager.h"

#include "IAsset.h"

using namespace OSK::ASSETS;

void AssetManager::DeleteLifetime(std::string_view lifetime) {
	if (!assetsPerLifetime.contains(lifetime))
		return;

	for (const auto& i : assetsPerLifetime.find(lifetime)->second)
		assetsTable.erase(i->GetAssetFilename());

	assetsPerLifetime.erase(assetsPerLifetime.find(lifetime));
}
