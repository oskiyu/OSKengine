#include "AudioDeviceAl.h"

#include "Assert.h"
#include "AudioExceptions.h"

#include "OSKengine.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::AUDIO;

std::string GetErrorCodeStr(ALCenum value) {
	switch (value) {
		case ALC_INVALID_DEVICE:	return "ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function.";
		case ALC_INVALID_CONTEXT:	return "ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function.";
		case ALC_INVALID_ENUM:		return "ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function.";
		case ALC_INVALID_VALUE:		return "ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function.";
		case ALC_OUT_OF_MEMORY:		return "ALC_OUT_OF_MEMORY.";

		default: return "UNKNOWN";
	};
}

void AudioDeviceAl::Initialize(const std::string& name) {
	Engine::GetLogger()->Log(IO::LogLevel::INFO, "Tratando de crear el Audio Device: ", name);
	m_device = UniquePtr<ALCdevice, ALCdeviceDeleter>(alcOpenDevice(name.data()));
	if (!m_device.HasValue()) {
		const auto error = alcGetError(nullptr);
		Engine::GetLogger()->Log(IO::LogLevel::L_ERROR, "Error al crear el device de OpenAL: ", GetErrorCodeStr(error));
	}
	OSK_ASSERT(m_device.HasValue(), AudioDeviceCreationException("Error al crear el device."));

	m_context = UniquePtr<ALCcontext, ALCcontextDeleter>(alcCreateContext(m_device.GetPointer(), nullptr));
	OSK_ASSERT(m_context.HasValue(), AudioDeviceCreationException("Error al crear el contexto."));

	SetName(name);
}

void AudioDeviceAl::InitializeDefault() {
	m_device = UniquePtr<ALCdevice, ALCdeviceDeleter>(alcOpenDevice(nullptr));
	m_context = UniquePtr<ALCcontext, ALCcontextDeleter>(alcCreateContext(m_device.GetPointer(), nullptr));

	SetName("Default device");
}

AudioDeviceAl::~AudioDeviceAl() {
	if (IsCurrentOutputDevice()) {
		alcMakeContextCurrent(nullptr);
	}
}

void AudioDeviceAl::ALCdeviceDeleter::operator()(ALCdevice* device) const noexcept {
	if (device) {
		alcCloseDevice(device);
	}
}

void AudioDeviceAl::ALCcontextDeleter::operator()(ALCcontext* context) const noexcept {
	if (context){
		alcDestroyContext(context);
	}
}

struct ALCcontextDeleter {
	void operator()(ALCcontext*) const noexcept;
};


void AudioDeviceAl::SetCurrentOutputDevice_Implementation() {
	const auto result = alcMakeContextCurrent(m_context.GetPointer());
	OSK_ASSERT(result == ALC_TRUE, AudioException("Error al establecer el contexto.", alcGetError(m_device.GetPointer())));
}
