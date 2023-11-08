#include "TreeNormalsRenderSystem.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "Transform3D.h"
#include "ModelComponent3D.h"
#include "ICommandList.h"
#include "IRenderer.h"
#include "GpuImageUsage.h"
#include "Viewport.h"

#include "CameraComponent3D.h"

#include "MatrixOperations.hpp"

#include "TreeNormalsPass.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


TreeNormalsRenderSystem::TreeNormalsRenderSystem() {
	// Signature del sistema
	Signature signature{};
	signature.SetTrue(Engine::GetEcs()->GetComponentType<Transform3D>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<ModelComponent3D>());
	_SetSignature(signature);

	AddRenderPass(new TreeNormalsPass());
}

void TreeNormalsRenderSystem::CreateBuffers() {
	IGpuMemoryAllocator* memAllocator = Engine::GetRenderer()->GetAllocator();

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		m_cameraBuffers[i] = memAllocator->CreateUniformBuffer(sizeof(CameraInfo)).GetPointer();
	}
}

void TreeNormalsRenderSystem::Initialize(GameObjectIndex camera) {
	m_cameraObject = camera;

	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_Default();
	const GpuImageViewConfig cubemapConfig = GpuImageViewConfig::CreateSampled_Cubemap();

	for (auto& pass : m_renderPasses) {
		pass->Load();
		pass->SetCamera(camera);
	}

	CreateBuffers();

	// Material setup
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> cameraBuffers{};
	for (UIndex64 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		cameraBuffers[i] = m_cameraBuffers[i].GetPointer();

	auto* materialInstance = GetRenderPass("tree_normals_pass")->GetMaterialInstance();
	materialInstance->GetSlot("global")->SetUniformBuffers("camera", cameraBuffers);
	materialInstance->GetSlot("global")->FlushUpdate();
}

void TreeNormalsRenderSystem::AddRenderPass(OwnedPtr<GRAPHICS::IRenderPass> pass) {
	IRenderSystem::AddRenderPass(pass);

	pass->SetCamera(m_cameraObject);
	pass->Load();
}

void TreeNormalsRenderSystem::CreateTargetImage(const Vector2ui& size) {
	// Imagen final.
	RenderTargetAttachmentInfo colorAttachment{};
	colorAttachment.format = Format::RGB10A2_UNORM;
	colorAttachment.usage = GpuImageUsage::COLOR | GpuImageUsage::SAMPLED;
	colorAttachment.name = "Tree Normals Target";

	RenderTargetAttachmentInfo depthAttachment{};
	depthAttachment.format = Format::D32_SFLOAT;
	depthAttachment.usage = GpuImageUsage::DEPTH;
	depthAttachment.name = "Tree Normals Depth";

	m_renderTarget.Create(size, { colorAttachment }, depthAttachment);
}

void TreeNormalsRenderSystem::Render(ICommandList* commandList) {
	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	const CameraComponent3D& camera = Engine::GetEcs()->GetComponent<CameraComponent3D>(m_cameraObject);
	const Transform3D& cameraTransform = Engine::GetEcs()->GetComponent<Transform3D>(m_cameraObject);

	CameraInfo currentCameraInfo{};
	currentCameraInfo.projectionMatrix = camera.GetProjectionMatrix();
	currentCameraInfo.viewMatrix = camera.GetViewMatrix(cameraTransform);
	currentCameraInfo.projectionViewMatrix = currentCameraInfo.projectionMatrix * currentCameraInfo.viewMatrix;
	currentCameraInfo.position = glm::vec4(cameraTransform.GetPosition().ToGlm(), 1.0f);
	currentCameraInfo.nearFarPlanes = { camera.GetNearPlane(), camera.GetFarPlane() };

	m_cameraBuffers[resourceIndex]->MapMemory();
	m_cameraBuffers[resourceIndex]->Write(currentCameraInfo);
	m_cameraBuffers[resourceIndex]->Unmap();

	UpdatePerPassObjectLists();

	// Pase
	commandList->StartDebugSection("Tree Normals", Color::Red);

	commandList->BeginGraphicsRenderpass(&m_renderTarget, Color::Black * 0.0f);
	SetupViewport(commandList);

	for (auto& pass : m_renderPasses) {
		const auto& objectList = m_objectsPerPass.find(pass->GetTypeName())->second;
		pass->RenderLoop(commandList, objectList, 0 /*TODO*/, m_renderTarget.GetSize());
	}

	commandList->EndGraphicsRenderpass(false);

	commandList->SetGpuImageBarrier(
		m_renderTarget.GetMainColorImage(resourceIndex),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));
	
	commandList->EndDebugSection();
}
