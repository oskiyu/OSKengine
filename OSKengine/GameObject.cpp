#include "GameObject.h"

#include "ECS.h"

using namespace OSK;

void GameObject::Spawn(EntityComponentSystem* ecs, const Vector3f& position, const Vector3f& axis, float angle, const Vector3f& size) {
	if (!hasBeenCreated) {
		ID = ecs->CreateGameObject();
		ECSsystem = ecs;
		ECSsystem->GameObjects.insert({ ID, this });
		OnCreate();
	}

	hasBeenCreated = true;

	Transform3D.SetPosition(position);
	Transform3D.RotateLocalSpace(angle, axis);
}

void GameObject::Remove() {
	if (ECSsystem) {
		ECSsystem->DestroyGameObject(ID);

		if (ECSsystem->GameObjects.find(ID) != ECSsystem->GameObjects.end())
			ECSsystem->GameObjects.erase(ID);
	}
}