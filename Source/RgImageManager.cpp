#include "RgImageManager.h"

#include "Uuid.h"
#include "RgImageDependency.h"
#include "IRenderer.h"
#include "RenderPass.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

RgImageManager::RgImageManager(IRenderer* renderer) : m_renderer(renderer) {

}

GdrImageUuid RgImageManager::RegisterImage(
	const RgImageRegisterInfo& info,
	RgRelativeSizeImageRegisterArgs size)
{
	return RegisterImage(
		info,
		RgImageSizeInfo{
			.type = RgImageSizeType::RELATIVE,
			.absoluteResolution = {},
			.relativeResolution = size.relativeResolution
		});
}

GdrImageUuid RgImageManager::RegisterImage(
	const RgImageRegisterInfo& info,
	RgAbsoluteSizeImageRegisterArgs size)
{
	return RegisterImage(
		info,
		RgImageSizeInfo{
			.type = RgImageSizeType::ABSOLUTE,
			.absoluteResolution = size.resolution,
			.relativeResolution = {}
		});
}

GdrImageUuid RgImageManager::RegisterImage(
	const RgImageRegisterInfo& info,
	RgImageSizeInfo size)
{

	const auto uuid = StaticUuidProvider::New<GdrImageUuid>();

	m_imageSizes[uuid] = size;
	m_imageInfos[uuid] = info;

	return uuid;
}

GdrImageUuid RgImageManager::RegisterExternalImage(GpuImage* image) {
	const auto uuid = StaticUuidProvider::New<GdrImageUuid>();
	m_imagesMap[uuid] = image;
	return uuid;
}

void RgImageManager::CreateImages(std::span<const RenderPassEntry> renderpasses) {

	// Almacena todas las dependencias de imágenes de
	// todos los pases.
	std::unordered_map<GdrImageUuid, DynamicArray<PartialRgImageDependency>>
		imageDependencies{};

	// Inicializamos las dependencias.
	for (const auto& pass : renderpasses) {
		for (const auto& dependency : pass.renderPass->GetPartialImageDependencies()) {
			if (!imageDependencies.contains(dependency.imageRefId)) {
				imageDependencies[dependency.imageRefId] = {};
			}

			imageDependencies[dependency.imageRefId].Insert(dependency);
		}
	}

	// Obtenemos todos los usos de cada imagen.
	std::unordered_map<GdrImageUuid, GpuImageUsage> imagesUses{};
	for (const auto& [imageId, dependencies] : imageDependencies) {
		for (const auto& dependency : dependencies) {

			GpuImageUsage usage = GpuImageUsage::UNKNOWN;

			switch (dependency.usage) {

			case GpuExclusiveImageUsage::COLOR:
				usage = GpuImageUsage::COLOR;
				break;

			case GpuExclusiveImageUsage::DEPTH:
				usage = GpuImageUsage::DEPTH;
				break;

			case GpuExclusiveImageUsage::SAMPLED:
				usage = GpuImageUsage::SAMPLED;
				break;

			case GpuExclusiveImageUsage::TRANSFER_SOURCE:
				usage = GpuImageUsage::TRANSFER_SOURCE;
				break;

			case GpuExclusiveImageUsage::TRANSFER_DESTINATION:
				usage = GpuImageUsage::TRANSFER_DESTINATION;
				break;

			case GpuExclusiveImageUsage::COMPUTE_OR_RT_TARGET:
				usage = GpuImageUsage::RT_TARGET_IMAGE;
				break;

			case GpuExclusiveImageUsage::STENCIL:
				usage = GpuImageUsage::STENCIL;
				break;

			}

			// TODO: array / cubemap.

			if (imagesUses.contains(imageId)) {
				imagesUses[imageId] = usage;
			}
			else {
				imagesUses[imageId] |= usage;
			}

		}
	}

	for (const auto& [uuid, info] : m_imageInfos) {
		Vector3ui resolution = Vector3ui::Zero;

		if (m_imageSizes[uuid].type == RgImageSizeType::ABSOLUTE) {
			resolution = m_imageSizes[uuid].absoluteResolution;
		}
		else {
			resolution = Vector3ui(
				static_cast<USize32>(glm::ceil(static_cast<float>(m_framebufferResolution.x) * m_imageSizes[uuid].relativeResolution.x)),
				static_cast<USize32>(glm::ceil(static_cast<float>(m_framebufferResolution.y) * m_imageSizes[uuid].relativeResolution.y)),
				static_cast<USize32>(glm::ceil(static_cast<float>(m_framebufferResolution.z) * m_imageSizes[uuid].relativeResolution.z))
			);
		}

		const auto createInfo = GpuImageCreateInfo{
			.resolution = resolution,
			.format = info.format,
			.usage = imagesUses[uuid],
			.dimension = info.dimension,
			.numLayers = info.numLayers,
			.msaaSamples = 1,
			.samplerDesc = {},
			.memoryType = GpuSharedMemoryType::GPU_ONLY,
			.tilingType = GpuImageTiling::OPTIMAL,
			.queueType = GpuQueueType::MAIN
		};

		auto image = m_renderer->GetAllocator()->CreateImage(createInfo);

		if (info.name) {
			image->SetDebugName(*info.name);
		}

		m_imagesMap[uuid] = image.GetPointer();
		m_images.Insert(std::move(image));
	}
}

GpuImage& RgImageManager::GetImage(GdrImageUuid uuid) {
	return *m_imagesMap.find(uuid)->second;
}

const GpuImage& RgImageManager::GetImage(GdrImageUuid uuid) const {
	return *m_imagesMap.find(uuid)->second;
}

void RgImageManager::SetFramebufferResolution(USize32 res) {
	m_framebufferResolution = {
		res,
		1u,
		1u
	};
}

void RgImageManager::SetFramebufferResolution(Vector2ui res) {
	m_framebufferResolution = {
		res.x,
		res.y,
		1u
	};
}

void RgImageManager::SetFramebufferResolution(Vector3ui res) {
	m_framebufferResolution = res;
}
