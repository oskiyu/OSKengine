// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#include "pch.h"
#include "CppUnitTest.h"

#include "../OSKengine2/ITopLevelCollider.h"
#include "../OSKengine2/SphereCollider.h"
#include "../OSKengine2/AxisAlignedBoundingBox.h"
#include "../OSKengine2/Transform3D.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::COLLISION;

TEST_CLASS(ColliderTests) {

public:

	/// @brief Dos esferas de radio 1 separadas por 0.9m.
	TEST_METHOD(SphereCollidersTrue) {
		const SphereCollider a = SphereCollider(1.0f);
		const SphereCollider b = SphereCollider(1.0f);

		const Vector3f aPos = Vector3f(0.0f, 0.0f, 0.0f);
		const Vector3f bPos = Vector3f(0.0f, 0.0f, 0.9f);

		const bool result = a.IsColliding(b, aPos, bPos);
		Assert::IsTrue(result);
	}

	/// @brief Dos esferas de radio 1 separadas por 1.01m.
	TEST_METHOD(SphereCollidersFalse) {
		const SphereCollider a = SphereCollider(1.0f);
		const SphereCollider b = SphereCollider(1.0f);

		const Vector3f aPos = Vector3f(0.0f, 0.0f, 0.0f);
		const Vector3f bPos = Vector3f(0.0f, 0.0f, 1.01f);

		Assert::IsFalse(a.IsColliding(b, aPos, bPos), L"Debería ser false");
	}

	/// @brief Dos cajas de radio 1 separadas por 0.9m.
	TEST_METHOD(AabbCollidersTrue) {
		const AabbCollider a = AabbCollider(Vector3f(1.0f));
		const AabbCollider b = AabbCollider(Vector3f(1.0f));

		const Vector3f aPos = Vector3f(0.0f, 0.0f, 0.0f);
		const Vector3f bPos = Vector3f(0.0f, 0.0f, 0.9f);

		Assert::IsTrue(a.IsColliding(b, aPos, bPos));
	}

	/// @brief Dos cajas de radio 1 separadas por 1.01m.
	TEST_METHOD(AabbCollidersFalse) {
		const AabbCollider a = AabbCollider(Vector3f(1.0f));
		const AabbCollider b = AabbCollider(Vector3f(1.0f));

		const Vector3f aPos = Vector3f(0.0f, 0.0f, 0.0f);
		const Vector3f bPos = Vector3f(0.0f, 0.0f, 1.01f);

		Assert::IsFalse(a.IsColliding(b, aPos, bPos), L"Debería ser false");
	}


	/// @brief Una caja de radio 1 separadas por 1.01m.
	TEST_METHOD(AabbPoint) {
		const AabbCollider a = AabbCollider(Vector3f(1.0f));

		const Vector3f aPos = Vector3f(0.0f);
		Vector3f point = Vector3f(1.01f);

		Assert::IsFalse(a.ContainsPoint(aPos, point), L"Debería ser false");
	}

	/// @brief Dos cajas de radio 1 separadas por 1.01m.
	TEST_METHOD(SpherePoint) {
		const AabbCollider a = AabbCollider(Vector3f(1.0f));
		const AabbCollider b = AabbCollider(Vector3f(1.0f));

		const Vector3f aPos = Vector3f(0.0f);
		const Vector3f bPos = Vector3f(1.01f);

		Assert::IsFalse(a.IsColliding(b, aPos, bPos), L"Debería ser false");
	}

};
