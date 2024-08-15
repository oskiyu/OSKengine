#pragma once

#include "ApiCall.h"
#include "UniquePtr.hpp"

#include "IComponentController.h"
#include "IComponentView.h"

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
	/// Maneja la interfaz de usuario y su conexión
	/// con el juego.
	class OSKAPI_CALL Editor {

	public:

		/// @brief Función que permite crear
		// un controlador para un tipo en concreto.
		using ControllerFactoryMethod = std::function<OwnedPtr<Controllers::IComponentController>(ECS::GameObjectIndex, void*, Views::IComponentView*)>;

		/// @brief Función que permite crear
		// una vista para un tipo en concreto.
		using ViewFactoryMethod = std::function<OwnedPtr<Views::IComponentView>(const Vector2f& size)>;

		/// @brief Crea el editor.
		/// @param rootUiElement Elemento raíz de interfaz del juego.
		/// @param windowSize Tamaño de la ventana.
		/// @post Añade la interfaz de usuario del editor
		/// al elemento raíz de interfaz.
		Editor(
			OSK::UI::IContainer* rootUiElement,
			const Vector2f& windowSize);

#pragma region Controller factory

		/// @brief Registra el método de creación
		/// de un tipo de controlador en concreto.
		/// @param componentName Nombre del componente
		/// con el que está relacionado el controlador.
		/// @param controllerFactoryMethod Método de creación.
		void RegisterController(
			std::string_view componentName,
			const ControllerFactoryMethod& controllerFactoryMethod);

		/// @brief Registra el método de creación.
		/// de un tipo de controlador en concreto.
		/// @tparam TComponentController Tipo del controlador.
		template <typename TComponentController>
			requires Controllers::IsComponentController<TComponentController>
		void RegisterController() {
			RegisterController(
				TComponentController::GetComponentTypeName(),
				[](ECS::GameObjectIndex obj, void* componentData, Views::IComponentView* view) {
					return new TComponentController(obj, componentData, view);
				});
		}

		/// @brief Registra el método de creación.
		/// @tparam TComponentController Tipo del controlador.
		/// @param factoryMethod Función de creación.
		template <typename TComponentController>
			requires Controllers::IsComponentController<TComponentController>
		void RegisterController(const ControllerFactoryMethod& factoryMethod) {
			RegisterController(TComponentController::GetComponentTypeName(), factoryMethod);
		}

#pragma endregion

#pragma region View factory

		/// @brief Registra el método de creación
		/// de un tipo de vista en concreto.
		/// @param componentName Nombre del componente
		/// con el que está relacionado la vista.
		/// @param viewFactoryMethod Método de creación.
		void RegisterView(
			std::string_view componentName,
			const ViewFactoryMethod& viewFactoryMethod);

		/// @brief Registra el método de creación
		/// de un tipo de vista en concreto.
		/// @tparam TComponentView Tipo de la vista.
		template <typename TComponentView>
			requires Views::IsComponentView<TComponentView>
		void RegisterView() {
			RegisterView(TComponentView::GetComponentTypeName(), [](const Vector2f& size) { return new TComponentView(size); });
		}

		/// @brief Registra el método de creación
		/// de un tipo de vista en concreto.
		/// @tparam TComponentView Tipo de la vista.
		/// @param factoryMethod Método de creación.
		template <typename TComponentView>
			requires Views::IsComponentView<TComponentView>
		void RegisterView(const ViewFactoryMethod& factoryMethod) {
			RegisterView(TComponentView::GetComponentTypeName(), factoryMethod);
		}

#pragma endregion

		/// @brief Actualiza los controladores
		/// y las vistas activas.
		/// @param ecs Clase ECS del motor.
		/// @param deltaTime Tiempo transcurrido
		/// desde la última actualización.
		void Update(
			ECS::EntityComponentSystem* ecs,
			TDeltaTime deltaTime);


		/// @brief Establece el objeto seleccionado
		/// en el editor.
		/// @param gameObject ID del objeto
		/// seleccionado. Puede ser vacío
		/// para indicar que se ha deseleccionado.
		void SetSelectedObject(ECS::GameObjectIndex gameObject);

		/// @brief Establece el sistema seleccionado
		/// en el editor.
		/// @param systemName Nombre del sistema seleccionado.
		/// Si está vacío, se considera que se ha deseleccionado
		/// el sistema anterior.
		void SetSelectedSystem(const std::string& systemName);

		/// @return Interfaz de usuario del editor.
		/// No es nullptr.
		UI::EditorUi* GetUi();

	private:

		struct ControllerViewPair {
			Controllers::IComponentController* controller;
			Views::IComponentView* view;
			bool isNewInsertion = false;
		};

		/// @brief Devuelve el controlador y la vista
		/// para el componente indicado.
		/// Si no existe, lo crea previamente.
		/// @param ecs Clase ECS del motor.
		/// @param type Tipo de componente a buscar.
		/// @return Controlador para
		/// el componente indicado.
		/// Si el tipo de componente no tiene enlazado
		/// una pareja view-controller, devuelve vacío.
		std::optional<ControllerViewPair> GetOrCreateControllerViewPair(
			ECS::EntityComponentSystem* ecs,
			ECS::ComponentType type);

		std::unordered_map<std::string, ControllerFactoryMethod, StringHasher, std::equal_to<>> m_controllersFactoryMethods;
		std::unordered_map<std::string, ViewFactoryMethod, StringHasher, std::equal_to<>> m_viewsFactoryMethods;

		std::unordered_map<ECS::ComponentType, UniquePtr<Controllers::IComponentController>> m_controllers;
		std::unordered_map<ECS::ComponentType, Views::IComponentView*> m_views;

		// Estado.

		ECS::GameObjectIndex m_selectedObject = ECS::GameObjectIndex::CreateEmpty();
		std::string m_selectedSystemName{};

		/// @brief Indica si el objeto seleccionado ha
		/// cambiado desde la última actualización.
		bool m_isSelectedObjectDirty = false;


		// UI & views.
		UI::EditorUi* m_editorUi;

	};

}
