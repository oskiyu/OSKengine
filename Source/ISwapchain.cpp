#include "ISwapchain.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuImage.h"
#include "ICommandList.h"
#include "CopyImageInfo.h"

#include "GpuImageLayout.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <ctime>

using namespace OSK;
using namespace OSK::GRAPHICS;


ISwapchain::ISwapchain(PresentMode mode, Format format, std::span<const UIndex32> queueIndices) : m_presentMode(mode), m_colorFormat(format){
	for (const auto index : queueIndices) {
		m_queueIndices.Insert(index);
	}
}

void ISwapchain::SetNumImagesInFlight(USize32 imageCount) {
	m_numImagesInFlight = imageCount;
}

void ISwapchain::SetImage(OwnedPtr<GpuImage> image, UIndex32 index) {
	m_images[index] = image.GetPointer();
}

std::span<const UIndex32> ISwapchain::GetQueueIndices() const {
	return m_queueIndices.GetFullSpan();
}

void ISwapchain::TakeScreenshot(std::string_view path) {
	// Obtenemos la imagen a guardar.
	GpuImage* swapchainImage = m_images[Engine::GetRenderer()->GetCurrentFrameIndex()].GetPointer();

	// Creamos una imagen intermedia con tiling lineal,
	// necesario para poder pasar los datos a la CPU.
	GpuImageCreateInfo intermediateImageInfo = GpuImageCreateInfo::CreateDefault2D(
		swapchainImage->GetSize2D(),
		swapchainImage->GetFormat(),
		GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION
	);
	intermediateImageInfo.memoryType = GpuSharedMemoryType::GPU_AND_CPU;
	intermediateImageInfo.tilingType = GpuImageTiling::LINEAL;
	intermediateImageInfo.msaaSamples = 1;
	intermediateImageInfo.samplerDesc = GpuImageSamplerDesc::CreateDefault_NoMipMap();

	UniquePtr<GpuImage> intermediateImage = Engine::GetRenderer()->GetAllocator()->CreateImage(intermediateImageInfo).GetPointer();


	// Comenzamos el proceso de copia.

	// Preferir NO usar cola de transferencia, para no tener que transferir la propiedad de la imagen.
	OwnedPtr<ICommandList> commandList = Engine::GetRenderer()->CreateSingleUseCommandList(GpuQueueType::MAIN);
	commandList->Reset();
	commandList->Start();

	commandList->SetGpuImageBarrier(
		swapchainImage,
		GpuImageLayout::TRANSFER_SOURCE,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_READ)
	);

	commandList->SetGpuImageBarrier(
		intermediateImage.GetPointer(),
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE)
	);

	CopyImageInfo copyInfo = CopyImageInfo::CreateDefault2D(swapchainImage->GetSize2D());
	commandList->RawCopyImageToImage(*swapchainImage, intermediateImage.GetPointer(), copyInfo);

	commandList->SetGpuImageBarrier(
		intermediateImage.GetPointer(),
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::ALL, GpuAccessStage::MEMORY_WRITE));

	commandList->Close();

	Engine::GetRenderer()->SubmitSingleUseCommandList(commandList);


	// Obtenemos los píxeles de la imagen lineal, leyendo la memoria directamente.

	DynamicArray<TByte> cpuImage = DynamicArray<TByte>::CreateReserved(
		intermediateImage->GetSize2D().x * intermediateImage->GetSize2D().y * 4
	);

	intermediateImage->GetBuffer()->MapMemory();
	const TByte* mappedImage = intermediateImage->GetBuffer()->GetMappedData();

	const TByte* pointer = mappedImage;
	for (UIndex32 y = 0; y < intermediateImage->GetSize2D().y; y++) {
		for (UIndex32 x = 0; x < intermediateImage->GetSize2D().x; x++) {
			// Debemos cambiar el formato:
			// BGRA -> RGBA
			cpuImage.Insert(pointer[2]); 
			cpuImage.Insert(pointer[1]);
			cpuImage.Insert(pointer[0]);
			cpuImage.Insert(pointer[3]);
			pointer += 4;
		}
	}

	intermediateImage->GetBuffer()->Unmap();


	// Obtenemos la fecha de la captura.

	const auto now = std::chrono::system_clock::now();
	const auto time = std::chrono::system_clock::to_time_t(now);
	std::tm now2{};
	localtime_s(&now2, &time);
	
	const std::string nowText = std::format(
		"{}_{}_{}_{}{}{}",
		now2.tm_year + 1900,
		now2.tm_mon + 1,
		now2.tm_mday,
		now2.tm_hour,
		now2.tm_min,
		now2.tm_sec);


	// Guardamos la imagen.

	const auto name = std::format("{}_{}.png", path, nowText);
	stbi_write_png(name.c_str(), intermediateImage->GetSize2D().x, intermediateImage->GetSize2D().y, 4, cpuImage.GetData(), intermediateImage->GetSize2D().x * 4);

}

PresentMode ISwapchain::GetCurrentPresentMode() const {
	return m_presentMode;
}

unsigned int ISwapchain::GetImageCount() const {
	return m_numImagesInFlight;
}

unsigned int ISwapchain::GetCurrentFrameIndex() const {
	return m_currentFrameIndex;
}

void ISwapchain::SetPresentMode(const IGpu& gpu, PresentMode mode) {
	m_presentMode = mode;
	// Recrear.
	Resize(gpu, GetImage(0)->GetSize2D());
}

Format ISwapchain::GetColorFormat() const {
	return m_colorFormat;
}
