#include "Editor.h"

#include "EntityComponentSystem.h"
#include "EditorUi.h"
#include "IComponentController.h"
#include "EditorObjectList.h"
#include "EditorSystemList.h"
#include "EditorPropertiesPanel.h"
#include "EditorObjectPropertiesPanel.h"
#include "EditorSystemPropertiesPanel.h"

using namespace OSK;
using namespace OSK::UI;
using namespace OSK::ECS;
using namespace OSK::Editor;
using namespace OSK::Editor::UI;

OSK::Editor::Editor::Editor(IContainer* rootUiElement, const Vector2f& windowSize) : m_editorUi(new EditorUi(windowSize, this)) {
	m_editorUi->SetAnchor(Anchor::FULLY_CENTERED);
	m_editorUi->SetKeepRelativeSize(true);
	m_editorUi->Rebuild();
	m_editorUi->SetInvisible();

	rootUiElement->AddChild(EditorUi::Name, m_editorUi);
}

OSK::Editor::UI::EditorUi* OSK::Editor::Editor::GetUi() {
	return m_editorUi;
}

void OSK::Editor::Editor::Update(EntityComponentSystem* ecs, TDeltaTime deltaTime) {
	m_ecs = ecs;

	UpdateByObject(ecs);
	UpdateBySystem();
	
	// Interfaz.
	m_editorUi->Update(ecs, deltaTime);
}

void OSK::Editor::Editor::UpdateByObject(EntityComponentSystem* ecs) {
	// Si el objeto seleccionado no está vivo,
	// se vacía.
	if (!ecs->IsGameObjectAlive(m_selectedObject)) {
		m_selectedObject = ECS::GameObjectIndex::CreateEmpty();
		m_isSelectedObjectDirty = true;
	}

	// Si el objeto seleccionado ha cambiado,
	// se eliminan los controladores y las
	// vistas relacionadas.
	if (m_isSelectedObjectDirty) {
		m_controllers.clear();
		m_views.clear();

		m_isSelectedObjectDirty = false;

		m_editorUi->ClearAllComponentViews();
	}

	// Si no hay objeto, no se actualiza.
	if (m_selectedObject.IsEmpty()) {
		m_editorUi->ClearAllComponentViews();

		return;
	}

	// Actualizar cada controlador de cada 
	// tipo de componente poseído.
	for (const ComponentType type : ecs->GetObjectComponentsTypes(m_selectedObject)) {
		auto controllerViewPairOpt = GetOrCreateControllerViewPair(ecs, type);

		if (!controllerViewPairOpt.has_value()) {
			continue;
		}

		auto& controllerViewPair = controllerViewPairOpt.value();

		if (controllerViewPair.isNewInsertion) {
			m_editorUi->AddComponentView(controllerViewPair.view);
		}

		// Actualizar la dirección de los
		// datos del componente.
		controllerViewPair.controller->Update(ecs->GetComponentAddress(m_selectedObject, type));

		// Proceso de actualización de:
		// - Componente (aplicando cambios en la interfaz).
		// - Interfaz (aplicando cambios en el componente).
		controllerViewPair.controller->Poll();
	}
}

void OSK::Editor::Editor::UpdateBySystem() {
	if (m_currentSystemController.HasValue()) {
		m_currentSystemController->Poll();
	}
}

std::optional<OSK::Editor::Editor::ControllerViewPair> OSK::Editor::Editor::GetOrCreateControllerViewPair(EntityComponentSystem* ecs, ECS::ComponentType type) {
	auto iterator = m_controllers.find(type);

	// Si no se encuentra...
	if (iterator == m_controllers.end()) {
		// ...crearlos.

		const std::string componentName = ecs->GetComponentTypeName(type);

		auto viewIterator = m_componentViewsFactoryMethods.find(componentName);
		if (viewIterator == m_componentViewsFactoryMethods.end()) {
			return std::optional<ControllerViewPair>{};
		}

		auto factoryIterator = m_componentControllersFactoryMethods.find(componentName);
		if (factoryIterator == m_componentControllersFactoryMethods.end()) {
			return std::optional<ControllerViewPair>{};
		}

		auto* view = viewIterator->second.operator()(Vector2f::One * 12.0f).GetPointer();
		m_views.emplace(type, view);

		const auto& factoryMethod = factoryIterator->second;

		auto controller = factoryMethod(
			m_selectedObject, 
			ecs->GetComponentAddress(m_selectedObject, type), 
			view).GetPointer();

		m_controllers[type] = controller;

		return ControllerViewPair{
			.controller = controller,
			.view = view,
			.isNewInsertion = true
		};
	}

	return ControllerViewPair{
		.controller = iterator->second.GetPointer(),
		.view = m_views.find(type)->second,
		.isNewInsertion = false
	};
}

void OSK::Editor::Editor::RegisterComponentController(std::string_view componentName, const ComponentControllerFactoryMethod& controller) {
	m_componentControllersFactoryMethods[static_cast<std::string>(componentName)] = controller;
}

void OSK::Editor::Editor::RegisterComponentView(std::string_view componentName, const ComponentViewFactoryMethod& view) {
	m_componentViewsFactoryMethods[static_cast<std::string>(componentName)] = view;
}

void OSK::Editor::Editor::RegisterSystemController(std::string_view systemName, const SystemControllerFactoryMethod& controller) {
	m_systemControllerFactoryMethods[static_cast<std::string>(systemName)] = controller;
}

void OSK::Editor::Editor::RegisterSystemView(std::string_view systemName, const SystemViewFactoryMethod& view) {
	m_systemViewsFactoryMethods[static_cast<std::string>(systemName)] = view;
}

void OSK::Editor::Editor::SetSelectedObject(ECS::GameObjectIndex gameObject) {
	m_isSelectedObjectDirty = true;
	m_selectedObject = gameObject;
}

void OSK::Editor::Editor::ClearSelectedObject() {
	SetSelectedObject(ECS::GameObjectIndex::CreateEmpty());
}

void OSK::Editor::Editor::SetSelectedSystem(const std::string& systemName) {
	m_selectedSystemName = systemName;

	if (systemName.empty()) {
		m_currentSystemController.Delete();
		m_editorUi->SetSelectedSystem(nullptr);
		return;
	}

	if (!m_systemControllerFactoryMethods.contains(systemName)) {
		m_currentSystemController.Delete();
		m_editorUi->SetSelectedSystem(m_ecs->GetSystemByName(systemName));

		return;
	}

	m_editorUi->SetSelectedSystem(m_ecs->GetSystemByName(systemName));

	const bool shouldCreateController =
		!m_currentSystemController.HasValue() ||
		m_currentSystemController->_GetSystemName() != systemName;

	if (!shouldCreateController) {
		return;
	}

	if (!m_systemControllerFactoryMethods.contains(systemName)) {
		return;
	}

	auto view = m_systemViewsFactoryMethods.find(systemName)->second(Vector2f::One * 12.0f);
	auto controller = m_systemControllerFactoryMethods.find(systemName)->second(
		m_ecs->GetSystemByName(systemName),
		view.GetPointer());

	m_currentSystemController = controller.GetPointer();

	m_editorUi->SetSystemPropertiesView(view.GetPointer());
}

void OSK::Editor::Editor::ClearSelectedSystem() {
	SetSelectedSystem("");
}
