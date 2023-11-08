#include "IRenderSystem.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "GpuImageDimensions.h"
#include "GpuImageUsage.h"
#include "GpuMemoryTypes.h"
#include "Format.h"
#include "Material.h"
#include "Viewport.h"

#include "ModelComponent3D.h"
#include "Model3D.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;


IRenderSystem::~IRenderSystem() {

}

void IRenderSystem::OnCreate() {
	// Si el renderizador está activo, se crean las imágenes.
	// Si no, se crearán cuando el renderizador se inicialice.
	if (Engine::GetRenderer()->IsOpen())
		CreateTargetImage(Engine::GetDisplay()->GetResolution());

	ISystem::OnCreate();
}

void IRenderSystem::Resize(const Vector2ui& windowSize) {
	m_renderTarget.Resize(windowSize);
}

void IRenderSystem::CreateTargetImage(const Vector2ui& size) {
	RenderTargetAttachmentInfo colorInfo{ .format = Format::RGBA8_UNORM };
	RenderTargetAttachmentInfo depthInfo{ .format = Format::D32S8_SFLOAT_SUINT };
	m_renderTarget.Create(size, { colorInfo }, depthInfo);
}

RenderTarget& IRenderSystem::GetRenderTarget() {
	return m_renderTarget;
}


void IRenderSystem::AddRenderPass(OwnedPtr<GRAPHICS::IRenderPass> pass) {
	m_renderPasses.Insert(pass.GetPointer());
	m_renderPassesTable[(std::string)pass->GetTypeName()] = pass.GetPointer();
	m_objectsPerPass[(std::string)pass->GetTypeName()] = {};
}

bool IRenderSystem::HasRenderPass(std::string_view name) const {
	return m_renderPassesTable.contains(name);
}

IRenderPass* IRenderSystem::GetRenderPass(std::string_view name) {
	return m_renderPassesTable.find(name)->second;
}

void IRenderSystem::UpdatePerPassObjectLists() {
	for (auto& [name, list] : m_objectsPerPass) {
		list.Empty();
	}

	for (const GameObjectIndex obj : GetObjects()) {
		const auto& model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj);

		const std::string_view passName = model.GetModel()->GetRenderPassType();

		if (!m_objectsPerPass.contains(passName))
			m_objectsPerPass[(std::string)passName] = {};

		auto& list = m_objectsPerPass.find(passName)->second;
		list.Insert(obj);
	}
}


void IRenderSystem::SetupViewport(GRAPHICS::ICommandList* commandList) {
	Vector4ui windowRec = {
		0,
		0,
		m_renderTarget.GetSize().x,
		m_renderTarget.GetSize().y
	};

	Viewport viewport{};
	viewport.rectangle = windowRec;

	commandList->SetViewport(viewport);
	commandList->SetScissor(windowRec);
}
