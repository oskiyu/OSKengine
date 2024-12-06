#pragma once

#include "ApiCall.h"
#include "UniquePtr.hpp"

#include "IComponentController.h"
#include "IComponentView.h"

#include "ISystemController.h"
#include "ISystemView.h"

#include "GameObject.h"
#include "Component.h"

#include "HashMap.hpp"
#include <functional>
#include "OwnedPtr.h"
#include "NumericTypes.h"

#include <string>
#include <optional>

namespace OSK::ECS {
	class EntityComponentSystem;
}

namespace OSK::Editor {

	namespace UI {
		class EditorUi;
	}


	/// @brief Clase principal del editor.
	/// Maneja la interfaz de usuario y su conexi�n
	/// con el juego.
	class OSKAPI_CALL Editor {

	public:

		/// @brief Funci�n que permite crear
		// un controlador para un tipo en concreto.
		using ComponentControllerFactoryMethod = std::function<OwnedPtr<Controllers::IComponentController>(ECS::GameObjectIndex, void*, Views::IComponentView*)>;

		/// @brief Funci�n que permite crear
		// una vista para un tipo en concreto.
		using ComponentViewFactoryMethod = std::function<OwnedPtr<Views::IComponentView>(const Vector2f& size)>;


		/// @brief Funci�n que permite crear
		// un controlador para un sistema en concreto.
		using SystemControllerFactoryMethod = std::function<OwnedPtr<Controllers::ISystemController>(ECS::ISystem*, Views::ISystemView*)>;

		/// @brief Funci�n que permite crear
		// una vista para un sistema en concreto.
		using SystemViewFactoryMethod = std::function<OwnedPtr<Views::ISystemView>(const Vector2f& size)>;


		/// @brief Crea el editor.
		/// @param rootUiElement Elemento ra�z de interfaz del juego.
		/// @param windowSize Tama�o de la ventana.
		/// @post A�ade la interfaz de usuario del editor
		/// al elemento ra�z de interfaz.
		Editor(
			OSK::UI::IContainer* rootUiElement,
			const Vector2f& windowSize);

#pragma region Component controller factory

		/// @brief Registra el m�todo de creaci�n
		/// de un tipo de controlador en concreto.
		/// @param componentName Nombre del componente
		/// con el que est� relacionado el controlador.
		/// @param controllerFactoryMethod M�todo de creaci�n.
		void RegisterComponentController(
			std::string_view componentName,
			const ComponentControllerFactoryMethod& controllerFactoryMethod);

		/// @brief Registra el m�todo de creaci�n.
		/// de un tipo de controlador en concreto.
		/// @tparam TComponentController Tipo del controlador.
		template <typename TComponentController>
			requires Controllers::IsComponentController<TComponentController>
		void RegisterComponentController() {
			RegisterComponentController(
				TComponentController::GetComponentTypeName(),
				[](ECS::GameObjectIndex obj, void* componentData, Views::IComponentView* view) {
					return new TComponentController(obj, componentData, view);
				});
		}

		/// @brief Registra el m�todo de creaci�n.
		/// @tparam TComponentController Tipo del controlador.
		/// @param factoryMethod Funci�n de creaci�n.
		template <typename TComponentController>
			requires Controllers::IsComponentController<TComponentController>
		void RegisterComponentController(const ComponentControllerFactoryMethod& factoryMethod) {
			RegisterComponentController(TComponentController::GetComponentTypeName(), factoryMethod);
		}

#pragma endregion

#pragma region Component view factory

		/// @brief Registra el m�todo de creaci�n
		/// de un tipo de vista en concreto.
		/// @param componentName Nombre del componente
		/// con el que est� relacionado la vista.
		/// @param viewFactoryMethod M�todo de creaci�n.
		void RegisterComponentView(
			std::string_view componentName,
			const ComponentViewFactoryMethod& viewFactoryMethod);

		/// @brief Registra el m�todo de creaci�n
		/// de un tipo de vista en concreto.
		/// @tparam TComponentView Tipo de la vista.
		template <typename TComponentView>
			requires Views::IsComponentView<TComponentView>
		void RegisterComponentView() {
			RegisterComponentView(TComponentView::GetComponentTypeName(), [](const Vector2f& size) { return new TComponentView(size); });
		}

		/// @brief Registra el m�todo de creaci�n
		/// de un tipo de vista en concreto.
		/// @tparam TComponentView Tipo de la vista.
		/// @param factoryMethod M�todo de creaci�n.
		template <typename TComponentView>
			requires Views::IsComponentView<TComponentView>
		void RegisterComponentView(const ComponentViewFactoryMethod& factoryMethod) {
			RegisterComponentView(TComponentView::GetComponentTypeName(), factoryMethod);
		}

#pragma endregion

#pragma region System controller factory

		/// @brief Registra el m�todo de creaci�n
		/// de un tipo de controlador en concreto.
		/// @param componentName Nombre del componente
		/// con el que est� relacionado el controlador.
		/// @param controllerFactoryMethod M�todo de creaci�n.
		void RegisterSystemController(
			std::string_view componentName,
			const SystemControllerFactoryMethod& controllerFactoryMethod);

		/// @brief Registra el m�todo de creaci�n.
		/// de un tipo de controlador en concreto.
		/// @tparam TComponentController Tipo del controlador.
		template <typename TSystemController>
			requires Controllers::IsSystemController<TSystemController>
		void RegisterSystemController() {
			RegisterSystemController(
				TSystemController::GetSystemName(),
				[](ECS::ISystem* system, Views::ISystemView* view) {
					return new TSystemController(system, view);
				});
		}

		/// @brief Registra el m�todo de creaci�n.
		/// @tparam TComponentController Tipo del controlador.
		/// @param factoryMethod Funci�n de creaci�n.
		template <typename TSystemController>
			requires Controllers::IsSystemController<TSystemController>
		void RegisterSystemController(const SystemControllerFactoryMethod& factoryMethod) {
			RegisterSystemController(TSystemController::GetSystemName(), factoryMethod);
		}

#pragma endregion

#pragma region Component view factory

		/// @brief Registra el m�todo de creaci�n
		/// de un tipo de vista en concreto.
		/// @param componentName Nombre del componente
		/// con el que est� relacionado la vista.
		/// @param viewFactoryMethod M�todo de creaci�n.
		void RegisterSystemView(
			std::string_view systemName,
			const SystemViewFactoryMethod& viewFactoryMethod);

		/// @brief Registra el m�todo de creaci�n
		/// de un tipo de vista en concreto.
		/// @tparam TComponentView Tipo de la vista.
		template <typename TSystemView>
			requires Views::IsSystemView<TSystemView>
		void RegisterSystemView() {
			RegisterSystemView(TSystemView::GetSystemName(), [](const Vector2f& size) { return new TSystemView(size); });
		}

		/// @brief Registra el m�todo de creaci�n
		/// de un tipo de vista en concreto.
		/// @tparam TComponentView Tipo de la vista.
		/// @param factoryMethod M�todo de creaci�n.
		template <typename TSystemView>
			requires Views::IsSystemView<TSystemView>
		void RegisterSystemView(const SystemViewFactoryMethod& factoryMethod) {
			RegisterSystemView(TSystemView::GetSystemName(), factoryMethod);
		}

#pragma endregion

		/// @brief Actualiza los controladores
		/// y las vistas activas.
		/// @param ecs Clase ECS del motor.
		/// @param deltaTime Tiempo transcurrido
		/// desde la �ltima actualizaci�n.
		void Update(
			ECS::EntityComponentSystem* ecs,
			TDeltaTime deltaTime);


		/// @brief Establece el objeto seleccionado
		/// en el editor.
		/// @param gameObject ID del objeto
		/// seleccionado. Puede ser vac�o
		/// para indicar que se ha deseleccionado.
		void SetSelectedObject(ECS::GameObjectIndex gameObject);

		/// @brief Establece que no haya ning�n objeto
		/// seleccionado.
		void ClearSelectedObject();

		/// @brief Establece el sistema seleccionado
		/// en el editor.
		/// @param systemName Nombre del sistema seleccionado.
		/// Si est� vac�o, se considera que se ha deseleccionado
		/// el sistema anterior.
		void SetSelectedSystem(const std::string& systemName);

		/// @brief Establece que no haya ning�n sistema
		/// seleccionado.
		void ClearSelectedSystem();

		/// @return Interfaz de usuario del editor.
		/// No es nullptr.
		UI::EditorUi* GetUi();

	private:

		struct ControllerViewPair {
			Controllers::IComponentController* controller;
			Views::IComponentView* view;
			bool isNewInsertion = false;
		};

		void UpdateByObject(ECS::EntityComponentSystem* ecs);
		void UpdateBySystem();

		/// @brief Devuelve el controlador y la vista
		/// para el componente indicado.
		/// Si no existe, lo crea previamente.
		/// @param ecs Clase ECS del motor.
		/// @param type Tipo de componente a buscar.
		/// @return Controlador para
		/// el componente indicado.
		/// Si el tipo de componente no tiene enlazado
		/// una pareja view-controller, devuelve vac�o.
		std::optional<ControllerViewPair> GetOrCreateControllerViewPair(
			ECS::EntityComponentSystem* ecs,
			ECS::ComponentType type);

		std::unordered_map<std::string, ComponentControllerFactoryMethod, StringHasher, std::equal_to<>> m_componentControllersFactoryMethods;
		std::unordered_map<std::string, ComponentViewFactoryMethod, StringHasher, std::equal_to<>> m_componentViewsFactoryMethods;

		std::unordered_map<std::string, SystemControllerFactoryMethod, StringHasher, std::equal_to<>> m_systemControllerFactoryMethods;
		std::unordered_map<std::string, SystemViewFactoryMethod, StringHasher, std::equal_to<>> m_systemViewsFactoryMethods;


		std::unordered_map<ECS::ComponentType, UniquePtr<Controllers::IComponentController>> m_controllers;
		std::unordered_map<ECS::ComponentType, Views::IComponentView*> m_views;

		UniquePtr<Controllers::ISystemController> m_currentSystemController;

		// Estado.

		ECS::EntityComponentSystem* m_ecs = nullptr;

		ECS::GameObjectIndex m_selectedObject = ECS::GameObjectIndex::CreateEmpty();
		std::string m_selectedSystemName{};

		/// @brief Indica si el objeto seleccionado ha
		/// cambiado desde la �ltima actualizaci�n.
		bool m_isSelectedObjectDirty = false;


		// UI & views.
		UI::EditorUi* m_editorUi;

	};

}
