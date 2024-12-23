#include "TransformApplierSystem.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "TransformComponent3D.h"

using namespace OSK;
using namespace OSK::ECS;

void TransformApplierSystem::OnCreate() {
	Signature signature{};
	signature.SetTrue(Engine::GetEcs()->GetComponentType<TransformComponent3D>());
	_SetSignature(signature);
}

void TransformApplierSystem::Execute(TDeltaTime, std::span<const GameObjectIndex> objects) {
	std::unordered_map<GameObjectIndex, GameObjectIndex> objectToParent;

	for (const GameObjectIndex obj : objects) {
		const auto& transform = Engine::GetEcs()->GetComponent<TransformComponent3D>(obj);
		
		for (const auto& child : transform.GetChildren()) {
			objectToParent[child] = obj;
		}
	}

	DynamicArray<GameObjectIndex> roots = DynamicArray<GameObjectIndex>::CreateReserved(objects.size());

	for (const GameObjectIndex obj : objects) {
		if (!objectToParent.contains(obj)) {
			roots.Insert(obj);
		}
	}

	for (const GameObjectIndex obj : roots) {
		Apply(Engine::GetEcs()->GetComponent<TransformComponent3D>(obj), {});
	}
}

void TransformApplierSystem::Apply(TransformComponent3D& transform, std::optional<const TransformComponent3D*> parent) {
	transform._ApplyChanges(parent);

	for (const auto& child : transform.GetChildren()) {
		auto& childTransform = Engine::GetEcs()->GetComponent<TransformComponent3D>(child);
		Apply(childTransform, &transform);
	}
}
