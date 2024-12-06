#include "IGpuImageSampler.h"

OSK::GRAPHICS::IGpuImageSampler::IGpuImageSampler(const GpuImageSamplerDesc& info) 
	: m_info(info) {

}

const OSK::GRAPHICS::GpuImageSamplerDesc& OSK::GRAPHICS::IGpuImageSampler::GetInfo() const {
	return m_info;
}
