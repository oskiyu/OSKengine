#include "GpuMemoryAllocatorVulkan.h"

#include "GpuMemoryBlockVulkan.h"
#include "GpuImageVulkan.h"
#include "Format.h"
#include <vulkan/vulkan.h>
#include "FormatVulkan.h"
#include "GpuVulkan.h"
#include "GpuMemoryTypeVulkan.h"
#include "GpuMemoryTypes.h"
#include "VertexVulkan.h"
#include "Vertex.h"
#include "IGpuDataBuffer.h"
#include "GpuVertexBufferVulkan.h"
#include "OSKengine.h"
#include "RendererVulkan.h"
#include "CommandListVulkan.h"
#include "GpuIndexBufferVulkan.h"
#include "GpuUniformBufferVulkan.h"
#include "GpuImageDimensions.h"
#include "GpuImageUsage.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

VkSamplerAddressMode GetAddressModeVulkan(GpuImageAddressMode mode) {
	switch (mode) {
	case OSK::GRAPHICS::GpuImageAddressMode::REPEAT:
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	case OSK::GRAPHICS::GpuImageAddressMode::MIRRORED_REPEAT:
		return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	case OSK::GRAPHICS::GpuImageAddressMode::EDGE:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	case OSK::GRAPHICS::GpuImageAddressMode::BACKGROUND:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	default:
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	}
}

VkFilter GetFilterTypeVulkan(GpuImageFilteringType type) {
	switch (type) {
	case OSK::GRAPHICS::GpuImageFilteringType::LIENAR:
		return VK_FILTER_LINEAR;
	case OSK::GRAPHICS::GpuImageFilteringType::NEAREST:
		return VK_FILTER_NEAREST;
	case OSK::GRAPHICS::GpuImageFilteringType::CUBIC:
		return VK_FILTER_CUBIC_IMG;
	default:
		return VK_FILTER_LINEAR;
	}
}

GpuMemoryAllocatorVulkan::GpuMemoryAllocatorVulkan(IGpu* device)
	: IGpuMemoryAllocator(device) {

}

OwnedPtr<IGpuVertexBuffer> GpuMemoryAllocatorVulkan::CreateVertexBuffer(const DynamicArray<Vertex3D>& vertices) {
	const TSize bufferSize = vertices.GetSize() * sizeof(Vertex3D);
	auto block = GetNextBufferMemoryBlock(bufferSize, GpuBufferUsage::VERTEX_BUFFER | GpuBufferUsage::TRANSFER_DESTINATION, GpuSharedMemoryType::GPU_ONLY);

	GpuDataBuffer* stagingBuffer = CreateStagingBuffer(bufferSize).GetPointer();
	stagingBuffer->MapMemory();
	stagingBuffer->Write(vertices.GetData(), bufferSize);
	stagingBuffer->Unmap();

	GpuVertexBufferVulkan* output = new GpuVertexBufferVulkan(block->GetNextMemorySubblock(bufferSize), bufferSize, 0);

	auto singleTimeCommandList = Engine::GetRenderer()->CreateSingleUseCommandList()->As<CommandListVulkan>();
	singleTimeCommandList->Reset();
	singleTimeCommandList->Start();

	singleTimeCommandList->CopyBuffer(stagingBuffer, output, bufferSize, 0, 0);

	singleTimeCommandList->Close();

	singleTimeCommandList->RegisterStagingBuffer(stagingBuffer);

	Engine::GetRenderer()->SubmitSingleUseCommandList(singleTimeCommandList);

	return output;
}

OwnedPtr<IGpuIndexBuffer> GpuMemoryAllocatorVulkan::CreateIndexBuffer(const DynamicArray<TIndexSize>& indices) {
	const TSize bufferSize = sizeof(TIndexSize) * indices.GetSize();
	auto block = GetNextBufferMemoryBlock(bufferSize, GpuBufferUsage::INDEX_BUFFER | GpuBufferUsage::TRANSFER_DESTINATION, GpuSharedMemoryType::GPU_ONLY);

	GpuDataBuffer* stagingBuffer = CreateStagingBuffer(bufferSize).GetPointer();
	stagingBuffer->MapMemory();
	stagingBuffer->Write(indices.GetData(), bufferSize);
	stagingBuffer->Unmap();

	GpuIndexBufferVulkan* output = new GpuIndexBufferVulkan(block->GetNextMemorySubblock(bufferSize), bufferSize, 0);

	auto singleTimeCommandList = Engine::GetRenderer()->CreateSingleUseCommandList()->As<CommandListVulkan>();
	singleTimeCommandList->Reset();
	singleTimeCommandList->Start();

	singleTimeCommandList->CopyBuffer(stagingBuffer, output, bufferSize, 0, 0);

	singleTimeCommandList->Close();

	singleTimeCommandList->RegisterStagingBuffer(stagingBuffer);

	Engine::GetRenderer()->SubmitSingleUseCommandList(singleTimeCommandList);

	return output;
}

OwnedPtr<IGpuUniformBuffer> GpuMemoryAllocatorVulkan::CreateUniformBuffer(TSize size) {
	auto block = GetNextBufferMemoryBlock(size, GpuBufferUsage::UNIFORM_BUFFER, GpuSharedMemoryType::GPU_AND_CPU);

	return new GpuUniformBufferVulkan(block->GetNextMemorySubblock(size), size, 0);;
}

OwnedPtr<GpuDataBuffer> GpuMemoryAllocatorVulkan::CreateStagingBuffer(TSize size) {
	return new GpuDataBuffer(GetNextBufferMemoryBlock(size,
		GpuBufferUsage::TRANSFER_SOURCE,
		GpuSharedMemoryType::GPU_AND_CPU)->GetNextMemorySubblock(size), size, 0);
}

OwnedPtr<GpuImage> GpuMemoryAllocatorVulkan::CreateImage(const Vector3ui& imageSize, GpuImageDimension dimension, TSize numLayers, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType, TSize msaaSamples, GpuImageSamplerDesc samplerDesc) {
	TSize numBytes = GetFormatNumberOfBytes(format);

	switch (dimension) {
		case OSK::GRAPHICS::GpuImageDimension::d1D: numBytes *= imageSize.X; break;
		case OSK::GRAPHICS::GpuImageDimension::d2D: numBytes *= imageSize.X * imageSize.Y; break;
		case OSK::GRAPHICS::GpuImageDimension::d3D: numBytes *= imageSize.X * imageSize.Y * imageSize.Z; break;
	}

	Vector3ui finalImageSize = imageSize;

	switch (dimension) {
		case OSK::GRAPHICS::GpuImageDimension::d1D: finalImageSize = { imageSize.X , 1, 1 }; break;
		case OSK::GRAPHICS::GpuImageDimension::d2D: finalImageSize = { imageSize.X , imageSize.Y, 1 }; break;
	}

	VkSampler sampler = VK_NULL_HANDLE;

	GpuImageVulkan* output = new GpuImageVulkan(imageSize, dimension, usage, numLayers, format);


	// ------ IMAGE ---------- //
	VkImage vkImage = VK_NULL_HANDLE;

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = (VkImageType)((TSize)dimension - 1);
	imageInfo.extent.width = finalImageSize.X;
	imageInfo.extent.height = finalImageSize.Y;
	imageInfo.extent.depth = finalImageSize.Z;
	imageInfo.mipLevels = GpuImage::GetMipLevels(finalImageSize.X, finalImageSize.Y);
	imageInfo.arrayLayers = numLayers;
	imageInfo.format = GetFormatVulkan(format);
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = GetGpuImageUsageVulkan(usage);
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = (VkSampleCountFlagBits)msaaSamples;
	imageInfo.flags = EFTraits::HasFlag(usage, GpuImageUsage::CUBEMAP) ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;

	VkResult result = vkCreateImage(device->As<GpuVulkan>()->GetLogicalDevice(),
		&imageInfo, nullptr, &vkImage);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear la imagen en la GPU.");

	output->SetImage(vkImage);

	auto block = GpuMemoryBlockVulkan::CreateNewImageBlock(output, device, sharedType, usage);
	output->SetBlock(block.GetPointer());

	imageMemoryBlocks.Insert(block.GetPointer());


	// ------ VIEW ---------- //

	VkImageView view = VK_NULL_HANDLE;

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = vkImage;
	viewInfo.viewType = (VkImageViewType)((TSize)dimension - 1);
	if (numLayers > 1)
		viewInfo.viewType = (VkImageViewType)((TSize)viewInfo.viewType + 3);
	if (EFTraits::HasFlag(usage, GpuImageUsage::CUBEMAP))
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
	viewInfo.format = GetFormatVulkan(format);
	viewInfo.subresourceRange.aspectMask = GetGpuImageAspectVulkan(usage);
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = GpuImage::GetMipLevels(imageSize.X, imageSize.Y);
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = numLayers;

	vkCreateImageView(device->As<GpuVulkan>()->GetLogicalDevice(),
		&viewInfo, nullptr, &view);

	output->SetView(view);


	// ------ SAMPLER ---------- //

	//Info del sampler.
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	//Filtro:
	//	VK_FILTER_LINEAR: suavizado.
	//	VK_FILTER_NEAREST: pixelado.
	samplerInfo.minFilter = GetFilterTypeVulkan(samplerDesc.filteringType);
	samplerInfo.magFilter = GetFilterTypeVulkan(samplerDesc.filteringType);
	//AddressMode: como se accede a la imagen con TexCoords fuera de los límites.
	samplerInfo.addressModeU = GetAddressModeVulkan(samplerDesc.addressMode);
	samplerInfo.addressModeV = GetAddressModeVulkan(samplerDesc.addressMode);
	samplerInfo.addressModeW = GetAddressModeVulkan(samplerDesc.addressMode);

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16.0f;

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = (float)GpuImage::GetMipLevels(imageSize.X, imageSize.Y);
	samplerInfo.mipLodBias = 0.0f;

	vkCreateSampler(device->As<GpuVulkan>()->GetLogicalDevice(),
		&samplerInfo, nullptr, &sampler);

	output->SetSampler(sampler);

	return output;
}

IGpuMemoryBlock* GpuMemoryAllocatorVulkan::GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	auto it = bufferMemoryBlocks.Find({ size, usage, sharedType });

	for (auto it : bufferMemoryBlocks) {
		if (it.first.usage == usage && it.first.sharedType == sharedType) {
			auto& list = it.second;

			for (auto& i : list)
				if (i->GetAvailableSpace() >= size)
					return i.GetPointer();

			auto i = GpuMemoryBlockVulkan::CreateNewBufferBlock(size, device, sharedType, usage);

			bufferMemoryBlocks.Insert({ size, usage, sharedType }, {});
			bufferMemoryBlocks.Get({ size, usage, sharedType }).Insert(i.GetPointer());

			return i.GetPointer();
		}
	}

	auto i = GpuMemoryBlockVulkan::CreateNewBufferBlock(size, device, sharedType, usage);

	bufferMemoryBlocks.Insert({ size, usage, sharedType }, {});
	bufferMemoryBlocks.Get({ size, usage, sharedType }).Insert(i.GetPointer());

	return i.GetPointer();

	if (it.IsEmpty()) {
		auto i = GpuMemoryBlockVulkan::CreateNewBufferBlock(size, device, sharedType, usage);

		bufferMemoryBlocks.Insert({ size, usage, sharedType }, {});
		bufferMemoryBlocks.Get({ size, usage, sharedType }).Insert(i.GetPointer());

		return i.GetPointer();
	}
	else {
		auto& list = it.GetValue().second;

		for (auto i : list)
			if (i->GetAvailableSpace() >= size)
				return i.GetPointer();

		auto i = GpuMemoryBlockVulkan::CreateNewBufferBlock(size, device, sharedType, usage);

		bufferMemoryBlocks.Insert({ size, usage, sharedType }, {});
		bufferMemoryBlocks.Get({ size, usage, sharedType }).Insert(i.GetPointer());

		return i.GetPointer();
	}

	return nullptr;
}
