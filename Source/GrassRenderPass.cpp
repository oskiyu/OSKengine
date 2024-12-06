#include "GrassRenderPass.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"

#include "CameraComponent3D.h"
#include "Transform3D.h"
#include "ModelComponent3D.h"
#include "Model3D.h"
#include "DeferredPushConstants.h"
#include "Texture.h"

#include "MatrixOperations.hpp"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


void GrassRenderPass::Load() {
	m_passMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/Mesh/Grass/grass_material.json");

	for (auto& buffer : m_windBuffers) {
		buffer = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(WindBuffer)).GetPointer();
	}

	for (UIndex32 i = 0; i < m_cameraInstances.size(); i++) {
		m_cameraInstances[i] = m_passMaterial->CreateInstance().GetPointer();

		IMaterialSlot* globalSlot = m_cameraInstances[i]->GetSlot("global");
		globalSlot->SetUniformBuffer("camera", *m_cameraBuffers[i]);
		globalSlot->SetUniformBuffer("windBuffer", m_windBuffers[i].GetValue());
		globalSlot->SetGpuImage(
			"windTexture", 
			Engine::GetAssetManager()->Load<Texture>("Resources/Assets/Textures/wind_texture.json")->GetTextureView2D(),
			GpuImageSamplerDesc::CreateDefault_NoMipMap());
		globalSlot->FlushUpdate();
	}	
}

void GrassRenderPass::Update(TDeltaTime deltaTime) {
	m_windOffset += Vector2f{ 0.1f, 0.1f } * deltaTime;
	m_time += deltaTime;
}

void GrassRenderPass::RenderLoop(ICommandList* commandList, const DynamicArray<ECS::GameObjectIndex>& objectsToRender, GlobalMeshMapping* globalMeshMapping, UIndex32 jitterIndex, Vector2ui resolution) {
	commandList->StartDebugSection("Grass Mesh Shaders", Color::Green);

	commandList->BindMaterial(*m_passMaterial);
	commandList->BindMaterialSlot(*m_cameraInstances[Engine::GetRenderer()->GetCurrentResourceIndex()]->GetSlot("global"));
	commandList->DrawMeshShader({ 1, 1, 1 });

	commandList->EndDebugSection();

	m_windBuffers[Engine::GetRenderer()->GetCurrentResourceIndex()]->MapMemory();
	m_windBuffers[Engine::GetRenderer()->GetCurrentResourceIndex()]->ResetCursor();
	m_windBuffers[Engine::GetRenderer()->GetCurrentResourceIndex()]->Write(Vector4f(m_windOffset.x, m_windOffset.y, m_time, 0.0f));
	m_windBuffers[Engine::GetRenderer()->GetCurrentResourceIndex()]->Unmap();
}
