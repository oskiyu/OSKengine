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

void IRenderSystem::Execute(TDeltaTime deltaTime, std::span<const GameObjectIndex> objects) {
	IIteratorSystem::Execute(deltaTime, objects);

	for (auto pass : m_shaderPasses.GetAllPasses()) {
		pass->Update(deltaTime);
	}

	for (auto pass : m_shadowsPasses.GetAllPasses()) {
		pass->Update(deltaTime);
	}
}

void IRenderSystem::Resize(const Vector2ui& windowSize) {
	m_renderTarget.Resize(windowSize);
}

void IRenderSystem::CreateTargetImage(const Vector2ui& size) {
	RenderTargetAttachmentInfo colorInfo{ .format = Format::RGBA8_UNORM };
	RenderTargetAttachmentInfo depthInfo{ .format = Format::D32S8_SFLOAT_SUINT };
	m_renderTarget.Create(size, { colorInfo }, depthInfo);
}

void IRenderSystem::OnRenderStart(GRAPHICS::ICommandList* commandList) {

}

void IRenderSystem::OnRenderEnd(GRAPHICS::ICommandList* commandList) {

}

RenderTarget& IRenderSystem::GetRenderTarget() {
	return m_renderTarget;
}


void IRenderSystem::AddShaderPass(UniquePtr<GRAPHICS::IShaderPass>&& pass) {
	m_shaderPasses.AddShaderPass(std::move(pass));
}

void IRenderSystem::AddShadowsPass(UniquePtr<GRAPHICS::IShaderPass>&& pass) {
	m_shadowsPasses.AddShaderPass(std::move(pass));
}

IShaderPass* IRenderSystem::GetShaderPass(std::string_view name) {
	if (m_shaderPasses.ContainsShaderPass(name)) {
		return m_shaderPasses.GetShaderPass(name);
	}

	if (m_shadowsPasses.ContainsShaderPass(name)) {
		return m_shadowsPasses.GetShaderPass(name);
	}

	return nullptr;
}

const IShaderPass* IRenderSystem::GetShaderPass(std::string_view name) const {
	if (m_shaderPasses.ContainsShaderPass(name)) {
		return m_shaderPasses.GetShaderPass(name);
	}

	if (m_shadowsPasses.ContainsShaderPass(name)) {
		return m_shadowsPasses.GetShaderPass(name);
	}

	return nullptr;
}

void IRenderSystem::UpdatePerPassObjectLists(std::span<const ECS::GameObjectIndex> objects) {
	for (auto& pass : m_shaderPasses.GetAllPasses()) {
		m_shaderPasses.GetCompatibleObjects(pass->GetTypeName()).Empty();
	}

	for (auto& pass : m_shadowsPasses.GetAllPasses()) {
		m_shadowsPasses.GetCompatibleObjects(pass->GetTypeName()).Empty();
	}


	for (const GameObjectIndex obj : objects) {
		const auto& model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj);

		for (const auto& passName : model.GetShaderPassNames()) {
			if (m_shaderPasses.ContainsShaderPass(passName)) {
				m_shaderPasses.AddCompatibleObject(passName, obj);
			}

			if (m_shadowsPasses.ContainsShaderPass(passName)) {
				m_shadowsPasses.AddCompatibleObject(passName, obj);
			}
		}		
	}
}

std::span<GRAPHICS::IShaderPass* const> IRenderSystem::GetAllShaderPasses() const {
	return m_shaderPasses.GetAllPasses();
}

std::span<GRAPHICS::IShaderPass* const> IRenderSystem::GetAllShadowsPasses() const {
	return m_shadowsPasses.GetAllPasses();
}


void IRenderSystem::UpdatePassesCamera(ECS::GameObjectIndex cameraObject) {
	for (auto& pass : m_shaderPasses.GetAllPasses()) {
		pass->SetCamera(cameraObject);
	}

	for (auto& pass : m_shadowsPasses.GetAllPasses()) {
		pass->SetCamera(cameraObject);
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
