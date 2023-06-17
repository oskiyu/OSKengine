#include "AudioLoader.h"

#include "AudioAsset.h"
#include "FileIO.h"

#include <fstream>

using namespace OSK;
using namespace OSK::ASSETS;

void AudioLoader::Load(const std::string& assetFilePath, IAsset** asset) {
	AudioAsset* output = (AudioAsset*)*asset;

	// Asset file.
	const nlohmann::json assetInfo = ValidateDescriptionFile(assetFilePath);

	std::string_view rawPath = assetInfo["raw_asset_path"];
	output->SetName(assetInfo["name"]);

	// Audio
	auto& buffer = output->GetBuffer();

	buffer.Init();

	std::ifstream file(rawPath.data(), std::ifstream::binary);

	char chunkId[5] = "\0";
	uint32_t size = 0;
	file.read(chunkId, 4);
	file.read((char*)&size, 4);

	chunkId[4] = '\0';

	file.read(chunkId, 4);

	chunkId[4] = '\0';

	file.read(chunkId, 4);
	file.read((char*)&size, 4);

	short formatTag = 0;
	short channels = 0;
	int samplesPerSec = 0;
	int averageBytesPerSec = 0;
	short blockAlign = 0;
	short bitsPerSample = 0;

	file.read((char*)&formatTag, 2);
	file.read((char*)&channels, 2);
	file.read((char*)&samplesPerSec, 4);
	file.read((char*)&averageBytesPerSec, 4);
	file.read((char*)&blockAlign, 2);
	file.read((char*)&bitsPerSample, 2);

	if (size > 16) {
		file.seekg((int)file.tellg() + (size - 16));
	}

	file.read(chunkId, 4);
	file.read((char*)&size, 4);

	chunkId[4] = '\0';

	unsigned char* data = new unsigned char[size];

	file.read((char*)data, size);

	buffer.Fill(
		data,
		size,
		(channels == 2) ? AUDIO::Format::STEREO16 : AUDIO::Format::MONO16,
		samplesPerSec
	);

	delete[] data;
	data = NULL;

	file.close();
}
