#include "GpuMemoryAllocatorVulkan.h"

#include "GpuMemoryBlockVulkan.h"
#include "GpuImageVulkan.h"
#include "Format.h"
#include <vulkan/vulkan.h>
#include "FormatVulkan.h"
#include "GpuVulkan.h"
#include "GpuMemoryTypeVulkan.h"

using namespace OSK;

GpuMemoryAllocatorVulkan::GpuMemoryAllocatorVulkan(IGpu* device)
	: IGpuMemoryAllocator(device) {

}

/*OwnedPtr<IGpuMemoryBlock> GpuMemoryAllocatorVulkan::CreateNewBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	TSize bSize = IGpuMemoryAllocator::SizeOfMemoryBlockInMb * 1000;
	if (size > bSize)
		bSize = size;

	return GpuMemoryBlockVulkan::CreateNewBufferBlock(bSize, device, sharedType, usage).GetPointer();
}*/

/*OwnedPtr<IGpuMemoryBlock> GpuMemoryAllocatorVulkan::CreateNewImageMemoryBlock(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage) {
	return GpuMemoryBlockVulkan::CreateNewImageBlock(image, device, type, imageUSage).GetPointer();
}

OwnedPtr<GpuDataBuffer> GpuMemoryAllocatorVulkan::CreateBuffer(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {

}*/

OwnedPtr<GpuImage> GpuMemoryAllocatorVulkan::CreateImage(unsigned int sizeX, unsigned int sizeY, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType) {
	const auto size = sizeX * sizeY * GetFormatNumberOfBytes(format);

	VkImage image = VK_NULL_HANDLE;
	VkImageView view = VK_NULL_HANDLE;
	VkSampler sampler = VK_NULL_HANDLE;

	GpuImageVulkan* output = new GpuImageVulkan(sizeX, sizeY, format);


	// ------ IMAGE ---------- //

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = sizeX;
	imageInfo.extent.height = sizeY;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = GpuImage::GetMipLevels(sizeX, sizeY);
	imageInfo.arrayLayers = 1;
	imageInfo.format = GetFormatVulkan(format);
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = GetGpuImageUsageVulkan(usage);
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = device->As<GpuVulkan>()->GetInfo().maxMsaaSamples;
	imageInfo.flags = 0;

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
	viewInfo.subresourceRange.levelCount = GpuImage::GetMipLevels(sizeX, sizeY);
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
	samplerInfo.maxLod = (float)GpuImage::GetMipLevels(sizeX, sizeY);
	samplerInfo.mipLodBias = 0.0f;

	vkCreateSampler(device->As<GpuVulkan>()->GetLogicalDevice(),
		&samplerInfo, nullptr, &sampler);

	output->SetView(view);
	output->SetSampler(sampler);

	return output;
}
