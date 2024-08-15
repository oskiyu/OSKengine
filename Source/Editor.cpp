#include "Editor.h"

#include "EntityComponentSystem.h"
#include "EditorUi.h"
#include "IComponentController.h"
#include "EditorObjectList.h"
#include "EditorPropertiesPanel.h"
#include "EditorObjectPropertiesPanel.h"

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

		m_editorUi
			->GetObjectListPanel()
			->GetPropertiesPanel()
			->ClearAllComponentViews();
	}

	// Si no hay objeto, no se actualiza.
	if (m_selectedObject.IsEmpty()) {
		m_editorUi
			->GetObjectListPanel()
			->GetPropertiesPanel()
			->ClearAllComponentViews();

		return;
	}

	// Actualizar cada controlador de cada 
	// tipo de componente poseído.
	for (const ComponentType type : ecs->GetObjectComponentsTypes(m_selectedObject)) {
		auto controllerViewPairOpt = GetOrCreateControllerViewPair(ecs, type);

		if (!controllerViewPairOpt.has_value()) {
			continue;
		}

		const auto& controllerViewPair = controllerViewPairOpt.value();

		if (controllerViewPair.isNewInsertion) {
			m_editorUi
				->GetObjectListPanel()
				->GetPropertiesPanel()
				->AddComponentView(controllerViewPair.view);

			m_editorUi
				->GetObjectListPanel()
				->GetPropertiesPanel()
				->Rebuild();
		}

		// Actualizar la dirección de los
		// datos del componente.
		controllerViewPair.controller->Update(ecs->GetComponentAddress(m_selectedObject, type));

		// Proceso de actualización de:
		// - Componente (aplicando cambios en la interfaz).
		// - Interfaz (aplicando cambios en el componente).
		controllerViewPair.controller->Poll();
	}

	// Interfaz.
	m_editorUi->Update(ecs, deltaTime);
}

std::optional<OSK::Editor::Editor::ControllerViewPair> OSK::Editor::Editor::GetOrCreateControllerViewPair(EntityComponentSystem* ecs, ECS::ComponentType type) {
	auto iterator = m_controllers.find(type);

	// Si no se encuentra...
	if (iterator == m_controllers.end()) {
		// ...crearlos.

		const std::string componentName = ecs->GetComponentTypeName(type);

		auto viewIterator = m_viewsFactoryMethods.find(componentName);
		if (viewIterator == m_viewsFactoryMethods.end()) {
			return std::optional<ControllerViewPair>{};
		}

		auto factoryIterator = m_controllersFactoryMethods.find(componentName);
		if (factoryIterator == m_controllersFactoryMethods.end()) {
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

void OSK::Editor::Editor::RegisterController(std::string_view componentName, const ControllerFactoryMethod& controller) {
	m_controllersFactoryMethods.emplace(static_cast<std::string>(componentName), controller);
}

void OSK::Editor::Editor::RegisterView(std::string_view componentName, const ViewFactoryMethod& view) {
	m_viewsFactoryMethods.emplace(static_cast<std::string>(componentName), view);
}

void OSK::Editor::Editor::SetSelectedObject(ECS::GameObjectIndex gameObject) {
	m_isSelectedObjectDirty = true;
	m_selectedObject = gameObject;
}

void OSK::Editor::Editor::SetSelectedSystem(const std::string& systemName) {
	m_selectedSystemName = systemName;
}
