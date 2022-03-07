#include "IAsset.h"

using namespace OSK::ASSETS;

IAsset::IAsset(const std::string& assetFile) 
	: assetFileName(assetFile) {

}

void IAsset::SetName(const std::string& name) {
	this->name = name;
}

std::string IAsset::GetName() const {
	return name;
}

std::string IAsset::GetAssetFilename() const {
	return assetFileName;
}
