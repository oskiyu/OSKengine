// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#include "pch.h"
#include "CppUnitTest.h"

#include "../OSKengine2/ITopLevelCollider.h"
#include "../OSKengine2/SphereCollider.h"
#include "../OSKengine2/AxisAlignedBoundingBox.h"
#include "../OSKengine2/Transform3D.h"

#include "../OSKengine2/ConvexVolume.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::COLLISION;

TEST_CLASS(ColliderTests) {

public:

	/// @brief Dos esferas de radio 1 separadas por 0.9m.
	TEST_METHOD(SphereCollidersTrue) {
		SphereCollider a = SphereCollider(1.0f);
		SphereCollider b = SphereCollider(1.0f);

		a.SetPosition(Vector3f(0.0f, 0.0f, 0.0f));
		b.SetPosition(Vector3f(0.0f, 0.0f, 0.9f));

		const bool result = a.IsColliding(b);
		Assert::IsTrue(result);
	}

	/// @brief Dos esferas de radio 1 separadas por 2.01m.
	TEST_METHOD(SphereCollidersFalse) {
		SphereCollider a = SphereCollider(1.0f);
		SphereCollider b = SphereCollider(1.0f);

		a.SetPosition(Vector3f(0.0f, 0.0f, 0.0f));
		b.SetPosition(Vector3f(0.0f, 0.0f, 2.01f));

		Assert::IsFalse(a.IsColliding(b));
	}

	/// @brief Dos cajas de radio 0.5 separadas por 0.9m.
	TEST_METHOD(AabbCollidersTrue) {
		AabbCollider a = AabbCollider(Vector3f(1.0f));
		AabbCollider b = AabbCollider(Vector3f(1.0f));

		a.SetPosition(Vector3f(0.0f, 0.0f, 0.0f));
		b.SetPosition(Vector3f(0.0f, 0.0f, 0.4f));

		Assert::IsTrue(a.IsColliding(b));
	}

	/// @brief Dos cajas de radio 1 separadas por 1.01m.
	TEST_METHOD(AabbCollidersFalse) {
		AabbCollider a = AabbCollider(Vector3f(1.0f));
		AabbCollider b = AabbCollider(Vector3f(1.0f));

		a.SetPosition(Vector3f(0.0f, 0.0f, 0.0f));
		b.SetPosition(Vector3f(0.0f, 0.0f, 1.01f));

		Assert::IsFalse(a.IsColliding(b));
	}


	/// @brief Una caja de radio 1 separadas por 1.01m.
	TEST_METHOD(AabbPoint) {
		AabbCollider a = AabbCollider(Vector3f(1.0f));

		a.SetPosition(Vector3f(0.0f));
		Vector3f point = Vector3f(1.01f);

		Assert::IsFalse(a.ContainsPoint(point), L"Debería ser false");
	}

	/// @brief Dos cajas de radio 1 separadas por 1.01m.
	TEST_METHOD(SpherePoint) {
		AabbCollider a = AabbCollider(Vector3f(1.0f));
		AabbCollider b = AabbCollider(Vector3f(1.0f));

		a.SetPosition(Vector3f::Zero);
		b.SetPosition(Vector3f::One);

		Assert::IsFalse(a.IsColliding(b), L"Deba ser false");
	}


	// --- Convex Volumes --- //

	TEST_METHOD(ConvexVolume_VertexDedup) {
		const DynamicArray<Vector3f> face = {
			OSK::Vector3f(1.0f, 0.0f, 1.0f),
			OSK::Vector3f(1.0f, 0.0f, -1.0f),
			OSK::Vector3f(-1.0f, 0.0f, 1.0f),
			OSK::Vector3f(-1.0f, 0.0f, -1.0f)
		};

		ConvexVolume volume = {};
		volume.AddFace(face);
		volume.AddFace(face);

		volume.MergeFaces();

		Assert::AreEqual(face.GetSize(), volume.GetVertices().GetSize());
	}

	TEST_METHOD(ConvexVolume_FaceDedup) {
		const DynamicArray<Vector3f> face = {
			OSK::Vector3f(1.0f, 0.0f, 1.0f),
			OSK::Vector3f(1.0f, 0.0f, -1.0f),
			OSK::Vector3f(-1.0f, 0.0f, 1.0f),
			OSK::Vector3f(-1.0f, 0.0f, -1.0f)
		};

		ConvexVolume volume = {};
		volume.AddFace(face);
		volume.AddFace(face);

		volume.MergeFaces();

		Assert::AreEqual(USize64(1), volume.GetAxes().GetSize());
	}

	TEST_METHOD(ConvexVolume_DeTriangulation) {
		const DynamicArray<Vector3f> triangleA = {
			OSK::Vector3f(1.0f, 0.0f, 1.0f),
			OSK::Vector3f(1.0f, 0.0f, -1.0f),
			OSK::Vector3f(-1.0f, 0.0f, 1.0f)
		};

		const DynamicArray<Vector3f> triangleB = {
			OSK::Vector3f(1.0f, 0.0f, -1.0f),
			OSK::Vector3f(-1.0f, 0.0f, 1.0f),
			OSK::Vector3f(-1.0f, 0.0f, -1.0f)
		};


		ConvexVolume volume = {};
		volume.AddFace(triangleA);
		volume.AddFace(triangleB);

		volume.MergeFaces();

		Assert::AreEqual(USize64(1), volume.GetAxes().GetSize());
		Assert::AreEqual(USize64(4), volume.GetVertices().GetSize());

		Assert::IsTrue(volume.GetVertices().ContainsElement(triangleA[0]));
		Assert::IsTrue(volume.GetVertices().ContainsElement(triangleA[1]));
		Assert::IsTrue(volume.GetVertices().ContainsElement(triangleA[2]));
		Assert::IsTrue(volume.GetVertices().ContainsElement(triangleB[2]));
	}

	/* TEST_METHOD(ConvexVolume_ContainsPoint) {
		auto volume = _BuildVolume(Vector3f::One);

		Assert::IsTrue(volume.ContainsPoint(Vector3f(0.49f)));
		Assert::IsFalse(volume.ContainsPoint(Vector3f(0.51f)));
	} */

	TEST_METHOD(ConvexVolume_RayCast) {
		auto volume = _BuildVolume(Vector3f(3.0f));

		Ray ray{};
		ray.direction = Vector3f(0.0f, 1.0f, 0.0f);
		ray.origin = Vector3f::Zero;

		auto result = volume.CastRay(ray);

		Assert::IsTrue(result.Result(), L"Result");
		Assert::IsTrue(result.GetIntersectionPoint().Equals(Vector3f(0.0f, 3.0f, 0.0f), 0.001f), L"Point");

		ray.direction = Vector3f(0.0f, 1.0f, 0.0f);
		ray.origin = Vector3f(0.0f, -5.0f, 0.0f);

		result = volume.CastRay(ray);

		Assert::IsTrue(result.Result(), L"Result2");
		Assert::IsTrue(result.GetIntersectionPoint().Equals(Vector3f(0.0f, -3.0f, 0.0f), 0.001f), L"Point2");

		ray.direction = Vector3f(0.0f, -1.0f, 0.0f);
		ray.origin = Vector3f(0.0f, 5.0f, 0.0f);

		result = volume.CastRay(ray);

		Assert::IsTrue(result.Result(), L"Result3");
		Assert::IsTrue(result.GetIntersectionPoint().Equals(Vector3f(0.0f, 3.0f, 0.0f), 0.001f), L"Point3");

		ray.direction = Vector3f(1.0f, 0.0f, 0.0f);
		ray.origin = Vector3f(-5.0f, 0.0f, 0.0f);

		result = volume.CastRay(ray);

		Logger::WriteMessage(result.GetIntersectionPoint().ToString().c_str());

		Assert::IsTrue(result.Result(), L"Result4");
		Assert::IsTrue(result.GetIntersectionPoint().Equals(Vector3f(-3.0f, 0.0f, 0.0f), 0.001f), L"Point4");
	}

	static ConvexVolume _BuildVolume(Vector3f size) {
		return ConvexVolume::CreateObb(size);
	}

};
