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

using namespace OSK;
using namespace OSK::GRAPHICS;

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

OwnedPtr<GpuImage> GpuMemoryAllocatorVulkan::CreateImage(const Vector2ui& imageSize, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType, bool singleSample) {
	const auto size = imageSize.X * imageSize.Y * GetFormatNumberOfBytes(format);

	VkImage image = VK_NULL_HANDLE;
	VkImageView view = VK_NULL_HANDLE;
	VkSampler sampler = VK_NULL_HANDLE;

	GpuImageVulkan* output = new GpuImageVulkan(imageSize.X, imageSize.Y, format);


	// ------ IMAGE ---------- //

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = imageSize.X;
	imageInfo.extent.height = imageSize.Y;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = GpuImage::GetMipLevels(imageSize.X, imageSize.Y);
	imageInfo.arrayLayers = 1;
	imageInfo.format = GetFormatVulkan(format);
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = GetGpuImageUsageVulkan(usage);
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.flags = 0;
	if (!singleSample)
		imageInfo.samples = device->As<GpuVulkan>()->GetInfo().maxMsaaSamples;
	else 
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

	VkResult result = vkCreateImage(device->As<GpuVulkan>()->GetLogicalDevice(), 
		&imageInfo, nullptr, &image);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo crear la imagen en la GPU.");

	output->SetImage(image);

	auto block = GpuMemoryBlockVulkan::CreateNewImageBlock(output, device, sharedType, usage);
	output->SetBlock(block.GetPointer());

	imageMemoryBlocks.Insert(block.GetPointer());


	// ------ VIEW ---------- //

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = GetFormatVulkan(format);
	viewInfo.subresourceRange.aspectMask = GetGpuImageAspectVulkan(usage);
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = GpuImage::GetMipLevels(imageSize.X, imageSize.Y);
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	vkCreateImageView(device->As<GpuVulkan>()->GetLogicalDevice(),
		&viewInfo, nullptr, &view);


	// ------ SAMPLER ---------- //

	//Info del sampler.
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	//Filtro:
	//	VK_FILTER_LINEAR: suavizado.
	//	VK_FILTER_NEAREST: pixelado.
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	//AddressMode: como se accede a la imagen con TexCoords fuera de los límites.
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

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

	output->SetView(view);
	output->SetSampler(sampler);

	return output;
}

IGpuMemoryBlock* GpuMemoryAllocatorVulkan::GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	auto it = bufferMemoryBlocks.Find({ size, usage, sharedType });

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
