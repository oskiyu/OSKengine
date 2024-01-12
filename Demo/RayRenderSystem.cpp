#include "RayRenderSystem.h"

#include <OSKengine/OSKengine.h>

#include <OSKengine/Transform3D.h>
#include <OSKengine/CameraComponent3D.h>

#include <OSKengine/Vertex3D.h>

#include "CircuitComponent.h"
#include "CarAiComponent.h"
#include "RayCastEvent.h"


RayRenderSystem::RayRenderSystem() {
	OSK::ECS::Signature signature{};
	signature.SetTrue(OSK::Engine::GetEcs()->GetComponentType<CircuitComponent>());
	_SetSignature(signature);

	m_pointsMaterial = OSK::Engine::GetRenderer()->GetMaterialSystem()
		->LoadMaterial("Resources/Materials/Collision/collision_point_material.json");
	m_pointsMaterialInstance = m_pointsMaterial->CreateInstance().GetPointer();

	m_linesMaterial = OSK::Engine::GetRenderer()->GetMaterialSystem()
		->LoadMaterial("Resources/Materials/Collision/ray_line_material.json");
	m_linesMaterialInstance = m_pointsMaterial->CreateInstance().GetPointer();


	// --- Camera buffers --- //
	std::array<const OSK::GRAPHICS::GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> cameraBuffers{};
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		m_cameraBuffers[i] = OSK::Engine::GetRenderer()->GetAllocator()
			->CreateUniformBuffer(sizeof(glm::mat4) * 2 + sizeof(glm::vec4))
			.GetPointer();
		cameraBuffers[i] = m_cameraBuffers[i].GetPointer();
	}

	m_pointsMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", cameraBuffers);
	m_pointsMaterialInstance->GetSlot("global")->FlushUpdate();

	m_linesMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", cameraBuffers);
	m_linesMaterialInstance->GetSlot("global")->FlushUpdate();


	// --- Points buffers --- //
	m_pointsVertexBuffer = OSK::Engine::GetRenderer()->GetAllocator()->CreateVertexBuffer<OSK::GRAPHICS::VertexCollisionDebug3D>(
		{ { OSK::Vector3f::Zero },{ OSK::Vector3f::Zero } }, OSK::GRAPHICS::VertexCollisionDebug3D::GetVertexInfo()).GetPointer();
	m_pointsIndexBuffer = OSK::Engine::GetRenderer()->GetAllocator()->CreateIndexBuffer({ 0, 1 }).GetPointer();
}

void RayRenderSystem::Initialize(OSK::ECS::GameObjectIndex cameraObject, OSK::ECS::GameObjectIndex carObject) {
	m_cameraObject = cameraObject;
	m_carObject = carObject;
}

void RayRenderSystem::CreateTargetImage(const OSK::Vector2ui& size) {
	const OSK::GRAPHICS::RenderTargetAttachmentInfo colorInfo{ .format = OSK::GRAPHICS::Format::RGBA8_SRGB, .name = "Ray Render System Target" };
	const OSK::GRAPHICS::RenderTargetAttachmentInfo depthInfo{ .format = OSK::GRAPHICS::Format::D16_UNORM, .name = "Ray Render System Depth" };
	m_renderTarget.Create(size, { colorInfo }, depthInfo);
}

void RayRenderSystem::SetDepthImages(OSK::GRAPHICS::GpuImage* image) {
	m_depthImage = { image, 0, false };
}

void RayRenderSystem::Render(OSK::GRAPHICS::ICommandList* commandList) {
// #define RENDER_ALL_CARS

	const UIndex32 resourceIndex = OSK::Engine::GetRenderer()->GetCurrentResourceIndex();

	if (m_cameraObject == OSK::ECS::EMPTY_GAME_OBJECT) {
		return;
	}

	const OSK::ECS::CameraComponent3D& camera = OSK::Engine::GetEcs()->GetComponent<OSK::ECS::CameraComponent3D>(m_cameraObject);
	const OSK::ECS::Transform3D& cameraTransform = OSK::Engine::GetEcs()->GetComponent<OSK::ECS::Transform3D>(m_cameraObject);

	m_cameraBuffers[resourceIndex]->MapMemory();
	m_cameraBuffers[resourceIndex]->Write(camera.GetProjectionMatrix());
	m_cameraBuffers[resourceIndex]->Write(camera.GetViewMatrix(cameraTransform));
	m_cameraBuffers[resourceIndex]->Write(cameraTransform.GetPosition());
	m_cameraBuffers[resourceIndex]->Unmap();

#ifndef RENDER_ALL_CARS
	const auto& carTransform = OSK::Engine::GetEcs()->GetComponent<OSK::ECS::Transform3D>(m_carObject);
#endif // !RENDER_ALL_CARS


	struct {
		OSK::Vector4f point;
		OSK::Color color = OSK::Color::Red;
	} pointPushConstant;

	struct {
		OSK::Vector4f pointA;
		OSK::Vector4f pointB;
		OSK::Color color = OSK::Color::Red;
	} linePushConstant;


	// Render loop

	commandList->StartDebugSection("Ray-cast debug", OSK::Color::Red);
	commandList->BeginGraphicsRenderpass(
		{ OSK::GRAPHICS::RenderPassImageInfo{m_renderTarget.GetColorImage(0), 0}},
		m_depthImage,
		OSK::Color::Black * 0.0f);

	SetupViewport(commandList);

	commandList->BindMaterial(*m_pointsMaterial);
	commandList->BindMaterialSlot(*m_pointsMaterialInstance->GetSlot("global"));

	commandList->BindVertexBuffer(*m_pointsVertexBuffer);
	commandList->BindIndexBuffer(*m_pointsIndexBuffer);

	for (const auto& event : OSK::Engine::GetEcs()->GetEventQueue<RayCastEvent>()) {
#ifndef RENDER_ALL_CARS
		if (event.car != m_carObject) {
			continue;
		}
#endif // !RENDER_ALL_CARS

#ifdef RENDER_ALL_CARS
		const auto& carTransform = OSK::Engine::GetEcs()->GetComponent<OSK::ECS::Transform3D>(event.car);
#endif // !RENDER_ALL_CARS

		const auto& result = event.rayResult;

		if (result.Result()) {
			pointPushConstant.point = OSK::Vector4f(
				result.GetIntersectionPoint().x,
				result.GetIntersectionPoint().y,
				result.GetIntersectionPoint().z,
				1.0f);

			const float distance = carTransform.GetPosition().GetDistanceTo(result.GetIntersectionPoint());

			pointPushConstant.color.red = glm::mix(1.0f, 0.0f, distance * 0.01f);
			pointPushConstant.color.green = glm::mix(0.0f, 1.0f, distance * 0.01f);

			commandList->PushMaterialConstants("pushConstants", pointPushConstant);
			commandList->DrawSingleInstance(1);
		}
	}

	commandList->BindMaterial(*m_linesMaterial);
	commandList->BindMaterialSlot(*m_linesMaterialInstance->GetSlot("global"));

	for (const auto& event : OSK::Engine::GetEcs()->GetEventQueue<RayCastEvent>()) {
#ifndef RENDER_ALL_CARS
		if (event.car != m_carObject) {
			continue;
	}
#endif // !RENDER_ALL_CARS
		
#ifdef RENDER_ALL_CARS
		const auto& carTransform = OSK::Engine::GetEcs()->GetComponent<OSK::ECS::Transform3D>(event.car);
#endif // !RENDER_ALL_CARS

		const auto& result = event.rayResult;

		linePushConstant.pointA = OSK::Vector4f(
			carTransform.GetPosition().x,
			carTransform.GetPosition().y + 0.15f,
			carTransform.GetPosition().z,
			1.0f);

		if (result.Result()) {
			linePushConstant.pointB = OSK::Vector4f(
				result.GetIntersectionPoint().x,
				result.GetIntersectionPoint().y,
				result.GetIntersectionPoint().z,
				1.0f);

			const float distance = carTransform.GetPosition().GetDistanceTo(result.GetIntersectionPoint());

			linePushConstant.color.red = glm::mix(1.0f, 0.0f, distance * 0.01f);
			linePushConstant.color.green = glm::mix(0.0f, 1.0f, distance * 0.01f);

			commandList->PushMaterialConstants("pushConstants", linePushConstant);
			commandList->DrawSingleInstance(2);
		}
		else {
			linePushConstant.color.red = 1.0f;
			linePushConstant.color.green = 0.0f;
		}
	}

	// Trazada
	for (const auto obj : GetObjects()) {
		linePushConstant.color = OSK::Color::Blue;

		const auto& spline = OSK::Engine::GetEcs()->GetComponent<CircuitComponent>(obj).trazadaIdeal->Get();
		const auto pointCount = spline.GetPointCount();

		for (UIndex64 i = 0; i < pointCount; i++) {
			linePushConstant.pointA = OSK::Vector4f(
				spline.GetPoint(i).x,
				spline.GetPoint(i).y + 0.5f,
				spline.GetPoint(i).z,
				1.0f);

			linePushConstant.pointB = OSK::Vector4f(
				spline.GetPoint((i + 1) % pointCount).x,
				spline.GetPoint((i + 1) % pointCount).y + 0.5f,
				spline.GetPoint((i + 1) % pointCount).z,
				1.0f);

			commandList->PushMaterialConstants("pushConstants", linePushConstant);
			commandList->DrawSingleInstance(2);
		}

#ifndef RENDER_ALL_CARS
		// Prox. punto
		const auto& ai = OSK::Engine::GetEcs()->GetComponent<CarAiComponent>(m_carObject);

		linePushConstant.pointA = OSK::Vector4f(
			carTransform.GetPosition().x,
			carTransform.GetPosition().y + 0.5f,
			carTransform.GetPosition().z,
			1.0f);

		/*linePushConstant.pointB = OSK::Vector4f(
			spline.GetPoint(ai.curentPointId).x,
			spline.GetPoint(ai.curentPointId).y + 0.5f,
			spline.GetPoint(ai.curentPointId).z,
			1.0f);*/

		linePushConstant.pointB = OSK::Vector4f(
			ai.nextPoint.x,
			ai.nextPoint.y + 0.5f,
			ai.nextPoint.z,
			1.0f);

		linePushConstant.pointB = linePushConstant.pointB; // +OSK::Vector4f(ai.diferencia);
		linePushConstant.pointB.y = 0.5f;

		linePushConstant.color = OSK::Color::Purple;

		commandList->PushMaterialConstants("pushConstants", linePushConstant);
		commandList->DrawSingleInstance(2);

		// Point
		commandList->BindMaterial(*m_pointsMaterial);
		commandList->BindMaterialSlot(*m_pointsMaterialInstance->GetSlot("global"));

		pointPushConstant.color = linePushConstant.color;
		pointPushConstant.point = linePushConstant.pointB;
		pointPushConstant.point.y = 0.5f;

		commandList->PushMaterialConstants("pushConstants", pointPushConstant);
		commandList->DrawSingleInstance(1);

#endif // RENDER_ALL_CARS		
	}

	commandList->EndGraphicsRenderpass();
	commandList->EndDebugSection();
}
