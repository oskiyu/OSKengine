#include "RgImageManager.h"

#include "Uuid.h"
#include "RgImageDependency.h"
#include "IRenderer.h"
#include "RenderPass.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

static std::unordered_map<GdrImageUuid, DynamicArray<PartialRgImageDependency>> GetDependenciesByImage(std::span<const RenderPassEntry> renderpasses) {
	std::unordered_map<GdrImageUuid, DynamicArray<PartialRgImageDependency>> output;

	for (const auto& pass : renderpasses) {
		for (const auto& dependency : pass.renderPass->GetPartialImageDependencies()) {
			output[dependency.imageRefId].Insert(dependency);
		}
	}

	return output;
}

static std::unordered_map<GdrImageUuid, GpuImageUsage> GetImagesUsages(const std::unordered_map<GdrImageUuid, DynamicArray<PartialRgImageDependency>>& alldependencies) {
	std::unordered_map<GdrImageUuid, GpuImageUsage> output{};

	for (const auto& [imageId, dependencies] : alldependencies) {
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

			output[imageId] |= usage;
		}
	}

	return output;
}

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

	Entry entry{};
	entry.size = size;
	entry.info = info;

	m_images[uuid] = std::move(entry);

	return uuid;
}

GdrImageUuid RgImageManager::RegisterExternalImage(GpuImage* image) {
	const auto uuid = StaticUuidProvider::New<GdrImageUuid>();

	Entry entry{};
	entry.img = image;
	entry.isExternal = true;

	m_images[uuid] = std::move(entry);

	return uuid;
}

void RgImageManager::RebindExternalImage(GdrImageUuid uuid, GpuImage* newImage) {
	m_images[uuid].img = newImage;
}

void RgImageManager::CreateImages(std::span<const RenderPassEntry> renderpasses) {
	// Almacena todas las dependencias de imágenes de
	// todos los pases.
	const auto imageDependencies = GetDependenciesByImage(renderpasses);

	// Obtenemos todos los usos de cada imagen.
	const auto  imagesUses = GetImagesUsages(imageDependencies);

	for (auto& [uuid, entry] : m_images) {
		if (entry.isExternal) {
			continue;
		}
		
		const auto createInfo = GpuImageCreateInfo{
			.resolution = GetResolution(entry.size),
			.format = entry.info.format,
			.usage = imagesUses.at(uuid),
			.dimension = entry.info.dimension,
			.numLayers = entry.info.numLayers,
			.msaaSamples = 1,
			.samplerDesc = {},
			.memoryType = GpuSharedMemoryType::GPU_ONLY,
			.tilingType = GpuImageTiling::OPTIMAL,
			.queueType = GpuQueueType::MAIN
		};

		auto image = m_renderer->GetAllocator()->CreateImage(createInfo);

		if (entry.info.name) {
			image->SetDebugName(*entry.info.name);
		}

		entry.img = image.GetPointer();
		entry.imgContainer = std::move(image);
	}
}

Vector3ui RgImageManager::GetResolution(const RgImageSizeInfo& size) const {
	if (size.type == RgImageSizeType::ABSOLUTE) {
		return size.absoluteResolution;
	}
	else {
		return Vector3ui(
			static_cast<USize32>(glm::ceil(static_cast<float>(m_framebufferResolution.x) * size.relativeResolution.x)),
			static_cast<USize32>(glm::ceil(static_cast<float>(m_framebufferResolution.y) * size.relativeResolution.y)),
			static_cast<USize32>(glm::ceil(static_cast<float>(m_framebufferResolution.z) * size.relativeResolution.z))
		);
	}
}

GpuImage& RgImageManager::GetImage(GdrImageUuid uuid) {
	return *m_images.find(uuid)->second.img;
}

const GpuImage& RgImageManager::GetImage(GdrImageUuid uuid) const {
	return *m_images.find(uuid)->second.img;
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

void RgImageManager::RecreateRelativeImages(std::span<const RenderPassEntry> renderpasses) {
	const auto imageDependencies = GetDependenciesByImage(renderpasses);
	const auto imagesUses = GetImagesUsages(imageDependencies);

	for (auto& [uuid, entry] : m_images) {
		if (!entry.isExternal && entry.size.type == RgImageSizeType::RELATIVE) {
			const auto createInfo = GpuImageCreateInfo{
				.resolution = GetResolution(entry.size),
				.format = entry.info.format,
				.usage = imagesUses.at(uuid),
				.dimension = entry.info.dimension,
				.numLayers = entry.info.numLayers,
				.msaaSamples = 1,
				.samplerDesc = {},
				.memoryType = GpuSharedMemoryType::GPU_ONLY,
				.tilingType = GpuImageTiling::OPTIMAL,
				.queueType = GpuQueueType::MAIN
			};

			entry.imgContainer = m_renderer->GetAllocator()->CreateImage(createInfo);
			entry.img = entry.imgContainer.GetPointer();
		}
	}
}
