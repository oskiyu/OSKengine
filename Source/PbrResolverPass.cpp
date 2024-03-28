#include "PbrResolvePass.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"

#include "CameraComponent3D.h"
#include "Transform3D.h"
#include "ModelComponent3D.h"
#include "Model3D.h"
#include "DeferredPushConstants.h"

#include "MatrixOperations.hpp"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


void PbrResolverPass::Load() {
	m_passMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/Deferred/deferred_resolve.json");
	m_materialInstance = m_passMaterial->CreateInstance().GetPointer();
}

void PbrResolverPass::RenderLoop(ICommandList* commandList, const DynamicArray<ECS::GameObjectIndex>& objectsToRender, GlobalMeshMapping* meshMapping, UIndex32 jitterIndex, Vector2ui resolution) {
	const Vector2ui threadGroupSize = { 8u, 8u };
	const Vector2ui dispatchRes = resolution / threadGroupSize + Vector2ui(1u, 1u);

	commandList->BindMaterial(*m_passMaterial);
	commandList->BindMaterialInstance(*m_materialInstance);
	commandList->PushMaterialConstants("taa", static_cast<int>(jitterIndex));
	commandList->DispatchCompute({ dispatchRes.x, dispatchRes.y, 1 });
}
