#pragma once

#include "IAsset.h"
#include "UniquePtr.hpp"
#include "AudioBuffer.h"

namespace OSK::ASSETS {

	class OSKAPI_CALL AudioAsset : public IAsset {

	public:

		AudioAsset(const std::string& assetFile);

		OSK_ASSET_TYPE_REG("OSK::AudioAsset")

		const AUDIO::Buffer& GetBuffer() const;
		AUDIO::Buffer& GetBuffer();

	private:

		AUDIO::Buffer buffer = {};

	};

}
