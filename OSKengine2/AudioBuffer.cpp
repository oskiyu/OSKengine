#include "AudioBuffer.h"

#include "Assert.h"
#include "AudioExceptions.h"
#include <AL/al.h>

using namespace OSK;

AUDIO::Buffer::~Buffer() {
	if (handle != EMPTY_HANDLE)
		alDeleteBuffers(1, &handle);
}

void AUDIO::Buffer::Init() {
	alGenBuffers(1, &handle);

	const auto result = alGetError();
	OSK_ASSERT(result == AL_NO_ERROR, AudioException("Error al crear buffer.", result));
}

void AUDIO::Buffer::Fill(const void* data, USize64 size, Format format, USize32 samplerate) {
	ALenum alFormat = 0;
	switch (format) {
	case OSK::AUDIO::Format::MONO8:
		alFormat = AL_FORMAT_MONO8;
		break;
	case OSK::AUDIO::Format::MONO16:
		alFormat = AL_FORMAT_MONO16;
		break;
	case OSK::AUDIO::Format::STEREO8:
		alFormat = AL_FORMAT_STEREO8;
		break;
	case OSK::AUDIO::Format::STEREO16:
		alFormat = AL_FORMAT_STEREO16;
		break;
	}

	alBufferData(handle, alFormat, data, size, samplerate);

	const auto result = alGetError();
	OSK_ASSERT(result == AL_NO_ERROR, AudioException("Error al poblar el buffer.", result));
}

AUDIO::Buffer::Handle AUDIO::Buffer::GetHandle() const {
	return handle;
}
