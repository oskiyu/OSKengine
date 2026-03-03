#include "RenderPass.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "MaterialSystem.h"
#include "RenderGraph.h"

#include "InvalidObjectStateException.h"
#include <format>

#include "Logger.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IRenderPass::IRenderPass(
	std::string_view materialName,
	RenderPassType type)
:
	m_type(type)
{
	m_material = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial(static_cast<std::string>(materialName));
	for (UIndex16 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		m_materialInstance[i] = m_material->CreateInstance();
	}
}

void IRenderPass::_Execute(ICommandList* cmdList, RenderGraph& renderGraph) {
	BindTargetImages(renderGraph);

	cmdList->StartDebugSection(
		static_cast<std::string>(GetName()),
		Color::Red);

	if (m_type == RenderPassType::GRAPHICS) {
		OSK_ASSERT_2(
			m_colorTargets,
			InvalidObjectStateException(std::format("No se ha establecido ningún target de color para el renderpass {}", GetName())),
			Engine::GetLogger());
		OSK_ASSERT_2(
			m_depthTarget,
			InvalidObjectStateException(std::format("No se ha establecido ningún target de profundidad para el renderpass {}", GetName())),
			Engine::GetLogger());

		cmdList->BeginGraphicsRenderpass(
			*m_colorTargets,
			*m_depthTarget,
			Color::Black,
			false);

		Vector4ui windowRec = {
			0,
			0,
			m_depthTarget->targetImage->GetSize2D().x,
			m_depthTarget->targetImage->GetSize2D().y
		};

		Viewport viewport{};
		viewport.rectangle = windowRec;

		cmdList->SetViewport(viewport);
		cmdList->SetScissor(windowRec);

		RebindGraphicsBuffers(cmdList, renderGraph);
	}

	if (m_needsResourceBinding) {
		BindResources(cmdList, renderGraph);
	}

	cmdList->BindMaterial(*m_material);
	cmdList->BindMaterialInstance(m_materialInstance[Engine::GetRenderer()->GetCurrentResourceIndex()].GetValue());

	Execute(cmdList);

	if (m_type == RenderPassType::GRAPHICS) {
		cmdList->EndGraphicsRenderpass(false);
	}

	cmdList->EndDebugSection();
}

void IRenderPass::BindResources(ICommandList* cmdList, RenderGraph& renderGraph) {

	for (USize16 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		// Dependencias de buffers.
		for (const auto& dependency : m_bufferDependencies) {
			if (dependency.frameInFlight && *dependency.frameInFlight != i) {
				continue;
			}

			if (dependency.usage == RgBufferUsage::STORAGE_BUFFER) {
				m_materialInstance[i]
					->GetSlot(dependency.shaderBinding.slotName)
					->SetStorageBuffer(
						dependency.shaderBinding.bindingName,
						renderGraph.GetBuffer(dependency.bufferRefId),
						dependency.range,
						dependency.arrayIndex);
			}
			else if (dependency.usage == RgBufferUsage::UNIFORM_BUFFER) {
				m_materialInstance[i]
					->GetSlot(dependency.shaderBinding.slotName)
					->SetUniformBuffer(
						dependency.shaderBinding.bindingName,
						renderGraph.GetBuffer(dependency.bufferRefId),
						dependency.range,
						dependency.arrayIndex);
			}
			else if (dependency.usage == RgBufferUsage::INDEX_BUFFER) {
				const auto& buffer = renderGraph.GetBuffer(dependency.bufferRefId);
				const auto view = IndexBufferView{
					.type = IndexType::U32,
					.numIndices = static_cast<USize32>(dependency.range.size / sizeof(TIndexSize)),
					.offsetInBytes = static_cast<USize32>(dependency.range.offset)
				};

				cmdList->BindIndexBufferRange(buffer, view);
			}
			else {
				// TODO: error handling
			}
		}


		// Dependencias de imágenes.
		for (const auto& dependency : m_imageDependencies) {
			if (dependency.frameInFlight && *dependency.frameInFlight != i) {
				continue;
			}

			const auto& image = renderGraph.GetImage(dependency.imageRefId);

			// Storage image.
			if (dependency.usage == GpuExclusiveImageUsage::COMPUTE_OR_RT_TARGET) {
				m_materialInstance[i]
					->GetSlot(dependency.shaderBinding.slotName)
					->SetStorageImage(
						dependency.shaderBinding.bindingName,
						*image.GetView(dependency.viewConfig),
						dependency.arrayIndex);
			}

			// Sampler.
			else if (dependency.usage == GpuExclusiveImageUsage::SAMPLED) {
				m_materialInstance[i]
					->GetSlot(dependency.shaderBinding.slotName)
					->SetGpuImage(
						dependency.shaderBinding.bindingName,
						*image.GetView(dependency.viewConfig),
						dependency.samplerDesc,
						dependency.arrayIndex);
			}
		}

		// Update de todos los recursos.
		for (const auto& [name, _] : m_materialInstance[i]->GetLayout()->GetAllSlots()) {
			m_materialInstance[i]->GetSlot(name)->FlushUpdate();
		}
	}

	// Ya se han establecido las instancias de los materiales.
	m_needsResourceBinding = false;
}

void IRenderPass::RebindGraphicsBuffers(ICommandList* cmdList, RenderGraph& renderGraph) {
	for (const auto& dependency : m_bufferDependencies) {

		if (dependency.usage == RgBufferUsage::VERTEX_BUFFER) {
			// TODO
		}		
		else if (dependency.usage == RgBufferUsage::INDEX_BUFFER) {
			const auto& buffer = renderGraph.GetBuffer(dependency.bufferRefId);
			const auto view = IndexBufferView{
				.type = IndexType::U32,
				.numIndices = static_cast<USize32>(dependency.range.size / sizeof(TIndexSize)),
				.offsetInBytes = static_cast<USize32>(dependency.range.offset)
			};

			cmdList->BindIndexBufferRange(buffer, view);
		}
	}
}

void IRenderPass::BindTargetImages(RenderGraph& renderGraph) {
	if (m_colorTargets) {
		m_colorTargets->Empty();
	}

	// Dependencias de imágenes.
	for (const auto& dependency : m_imageDependencies) {
		auto& image = renderGraph.GetImage(dependency.imageRefId);

		// Target de rasterizado.
		if (dependency.usage == GpuExclusiveImageUsage::COLOR) {
			RenderPassImageInfo info{};
			info.arrayLevel = dependency.arrayIndex;
			info.clear = dependency.clearIfTargetImage;
			info.targetImage = &image;

			if (!m_colorTargets.has_value()) {
				m_colorTargets = { info };
			}
			else {
				m_colorTargets->Insert(info);
			}
		}

		// Target de profundidad.
		else if (dependency.usage == GpuExclusiveImageUsage::DEPTH) {
			RenderPassImageInfo info{};
			info.arrayLevel = dependency.arrayIndex;
			info.clear = dependency.clearIfTargetImage;
			info.targetImage = &image;

			m_depthTarget = info;
		}
	}
}

std::span<const RgBufferDependency> IRenderPass::GetBufferDependencies() const {
	return m_bufferDependencies.GetFullSpan();
}
std::span<const RgImageDependency> IRenderPass::GetImageDependencies() const {
	return m_imageDependencies.GetFullSpan();
}

std::span<const PartialRgBufferDependency> IRenderPass::GetPartialBufferDependencies() const {
	return m_partialBufferDependencies.GetFullSpan();
}
std::span<const PartialRgImageDependency> IRenderPass::GetPartialImageDependencies() const {
	return m_partialImageDependencies.GetFullSpan();
}

void IRenderPass::AddBufferDependency(const RgBufferDependency& dependency) {
	m_bufferDependencies.Insert(dependency);
	m_needsResourceBinding = true;

	// Nos aseguramos que esté en la lista
	// que va a leer el rendergraph para la
	// creación de recursos.
	AddPartialBufferDependency({
		.bufferRefId = dependency.bufferRefId,
		.usage = dependency.usage
		});
}

void IRenderPass::AddImageDependency(const RgImageDependency& dependency) {
	m_imageDependencies.Insert(dependency);
	m_needsResourceBinding = true;

	// Nos aseguramos que esté en la lista
	// que va a leer el rendergraph para la
	// creación de recursos.
	AddPartialImageDependency({
		.imageRefId = dependency.imageRefId,
		.usage = dependency.usage
		});
}

void IRenderPass::AddPartialBufferDependency(const PartialRgBufferDependency& dependency) {
	m_partialBufferDependencies.Insert(dependency);
	m_needsResourceBinding = true;
}

void IRenderPass::AddPartialImageDependency(const PartialRgImageDependency& dependency) {
	m_partialImageDependencies.Insert(dependency);
	m_needsResourceBinding = true;
}

RenderPassType IRenderPass::GetType() const {
	return m_type;
}
