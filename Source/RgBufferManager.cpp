#include "RgBufferManager.h"

#include "Uuid.h"
#include "RgBufferDependency.h"
#include "IRenderer.h"
#include "RenderPass.h"

#include "OSKengine.h"
#include "Assert.h"
#include "InvalidArgumentException.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


static GpuBufferUsage GetUsage(RgBufferUsage usage) {
	switch (usage) {
	case OSK::GRAPHICS::RgBufferUsage::NONE:
		break;
	case OSK::GRAPHICS::RgBufferUsage::UNIFORM_BUFFER:
		return GpuBufferUsage::UNIFORM_BUFFER;
	case OSK::GRAPHICS::RgBufferUsage::STORAGE_BUFFER:
		return GpuBufferUsage::STORAGE_BUFFER;
	case OSK::GRAPHICS::RgBufferUsage::VERTEX_BUFFER:
		return GpuBufferUsage::VERTEX_BUFFER;
	case OSK::GRAPHICS::RgBufferUsage::INDEX_BUFFER:
		return GpuBufferUsage::INDEX_BUFFER;
	case OSK::GRAPHICS::RgBufferUsage::TRANSFER_SRC:
		return GpuBufferUsage::TRANSFER_SOURCE;
	case OSK::GRAPHICS::RgBufferUsage::TRANSFER_DST:
		return GpuBufferUsage::TRANSFER_DESTINATION;
	default:
		break;
	}
}


RgBufferManager::RgBufferManager(IRenderer* renderer) : m_renderer(renderer) {

}

GdrBufferUuid RgBufferManager::RegisterBuffer(const RgBufferRegisterInfo& info) {
	return RegisterBuffer(info, ReserveUuid());
}

GdrBufferUuid RgBufferManager::RegisterBuffer(const RgBufferRegisterInfo& info, GdrBufferUuid reservedUuid) {
	m_bufferInfos[reservedUuid] = info;
	return reservedUuid;
}

void RgBufferManager::AddBufferUse(GdrBufferUuid uuid, RgBufferUsage usage) {
	m_bufferUses[uuid].Insert({
		.bufferRefId = uuid,
		.usage = usage
		});
}

GdrBufferUuid RgBufferManager::ReserveUuid() {
	return StaticUuidProvider::New<GdrBufferUuid>();
}

std::unordered_map<GdrBufferUuid, DynamicArray<PartialRgBufferDependency>> RgBufferManager::GetBufferDependencies(std::span<const RenderPassEntry> renderpasses) {
	std::unordered_map<GdrBufferUuid, DynamicArray<PartialRgBufferDependency>> bufferDependencies{};

	// Inicializamos las dependencias.
	for (const auto& pass : renderpasses) {
		for (const auto& dependency : pass.renderPass->GetPartialBufferDependencies()) {
			bufferDependencies[dependency.bufferRefId].Insert(dependency);

			Engine::GetLogger()->DebugLog(std::format("GetBufferDependencies(): Buffer {} con uso {}", dependency.bufferRefId.Get(), (int)dependency.usage));
		}
	}

	// Dependencias locales.
	for (const auto& [uuid, uses] : m_bufferUses) {
		for (const auto& use : uses) {
			bufferDependencies[uuid].Insert(use);

			Engine::GetLogger()->DebugLog(std::format("GetBufferDependencies(): Buffer {} con uso {}", uuid.Get(), (int)use.usage));
		}
	}

	return bufferDependencies;
}

std::unordered_map<GdrBufferUuid, GpuBufferUsage> RgBufferManager::GetBufferUses(const std::unordered_map<GdrBufferUuid, DynamicArray<PartialRgBufferDependency>>& dependencies) {
	std::unordered_map<GdrBufferUuid, GpuBufferUsage> bufferUses{};
	
	for (const auto& [bufferId, bufferDependencies] : dependencies) {
		for (const auto& dependency : bufferDependencies) {
			bufferUses[bufferId] |= GetUsage(dependency.usage);

			Engine::GetLogger()->DebugLog(std::format("GetBufferUses(): Buffer {} con uso {}", bufferId.Get(), (int)dependency.usage));
		}
	}

	return bufferUses;
}

void RgBufferManager::CreateBuffers(std::span<const RenderPassEntry> renderpasses) {
	// Almacena todas las dependencias de buffers de
	// todos los pases.
	const auto dependencies = GetBufferDependencies(renderpasses);

	// Obtenemos todos los usos de cada buffer.
	const auto uses = GetBufferUses(dependencies);

	// Los buffers de geometría pueden no tener ninguna dependencia
	// si no se ha registrado un pase que los use.
	// 
	// Aun así, debemos añadir algunos de los valores para evitar
	// errores en tiempo de ejecución
	for (const auto& [uuid, info] : m_bufferInfos) {
		auto use = uses.at(uuid);

		OSK_ASSERT(
			std::to_underlying(use),
			InvalidArgumentException(std::format("Buffer {} sin usos registrados.", uuid.Get())));

		auto buffer = m_renderer->GetAllocator()->CreateBuffer(
			info.size,
			m_renderer->GetAllocator()->GetAlignment(info.alignment, use),
			use,
			info.sharedType,
			info.queue);

		m_buffersMap[uuid] = buffer.GetPointer();
		m_buffers.Insert(std::move(buffer));
	}

}

GpuBuffer& RgBufferManager::GetBuffer(GdrBufferUuid uuid) {
	auto it = m_buffersMap.find(uuid);

	OSK_ASSERT_2(
		it != m_buffersMap.end(),
		InvalidArgumentException(std::format("No existe el buffer con UUID {}", uuid.Get())),
		Engine::GetLogger());

	return *it->second;
}

const GpuBuffer& RgBufferManager::GetBuffer(GdrBufferUuid uuid) const {
	auto it = m_buffersMap.find(uuid);

	OSK_ASSERT_2(
		it != m_buffersMap.end(),
		InvalidArgumentException(std::format("No existe el buffer con UUID {}", uuid.Get())),
		Engine::GetLogger());

	return *it->second;
}
