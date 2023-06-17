#include "AudioAsset.h"

using namespace OSK;
using namespace OSK::ASSETS;

AudioAsset::AudioAsset(const std::string& assetFile) : IAsset(assetFile) {

}

const AUDIO::Buffer& AudioAsset::GetBuffer() const {
	return buffer;
}

AUDIO::Buffer& AudioAsset::GetBuffer() {
	return buffer;
}
