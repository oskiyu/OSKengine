#include "AssetLoaderJob.h"

#include "IAssetLoader.h"

using namespace OSK;
using namespace OSK::ASSETS;

AssetLoaderJob::AssetLoaderJob(const std::string& assetPath, IAssetLoader* loader) : m_assetPath(assetPath), m_loader(loader) {

}

void AssetLoaderJob::Execute() {
	m_loader->Load(m_assetPath);
	m_loader->SetAsLoaded(m_assetPath);
}
