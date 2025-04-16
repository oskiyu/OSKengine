#include "AssetManager.h"

#include "IAsset.h"

#include "OSKengine.h"
#include "JobSystem.h"
#include "AssetLoaderJob.h"

using namespace OSK::ASSETS;


void AssetManager::LaunchAsyncLoad(const std::string& assetPath, IAssetLoader* loader, std::span<const std::string> tags) {
	UniquePtr<AssetLoaderJob> job = MakeUnique<AssetLoaderJob>(assetPath, loader);
	
	for (const auto& tag : tags) {
		job->AddTag(tag);
	}

	OSK::Engine::GetJobSystem()->ScheduleJob(std::move(job));
}
