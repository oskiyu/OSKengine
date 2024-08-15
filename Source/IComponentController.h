#pragma once

#include "ApiCall.h"

#include "GameObject.h"
#include "Component.h"
#include <type_traits>

namespace OSK::Editor::Views {
	class IComponentView;
}

namespace OSK::Editor::Controllers {

	/// @brief Elemento que conecta los datos de un componente
	/// con un elemento de interfaz de usuario del editor correspondiente
	/// a ese componente.
	class OSKAPI_CALL IComponentController {

	public:

		virtual ~IComponentController() = default;

		/// @brief Actualiza la interfaz de usuario
		/// a partir de los datos del componente.
		virtual void Poll() = 0;

		/// @brief Actualiza la dirección de los datos
		/// del componente.
		/// @param component Dirección del componente.
		void Update(void* component);

		/// @return Objeto que posee el componente
		/// observado.
		ECS::GameObjectIndex GetLinkedObject() const;

		/// @return Tipo de componente observado.
		ECS::ComponentType GetLinkedComponentType() const;

	protected:

		/// @brief Inicializa los valores del controlador.
		/// @param object Objeto enlazado.
		/// @param componentData Dirección del componente.
		/// @param view Vista enlazada.
		/// 
		/// @pre @p object debe ser un objeto válido.
		IComponentController(
			ECS::GameObjectIndex object,
			void* componentData,
			Views::IComponentView* view);

		/// @return Vista enlazada con el controlador.
		Views::IComponentView* GetView();
		const Views::IComponentView* GetView() const;

		/// @return Dirección en la que se almacena el componente.
		void* GetComponentData();
		const void* GetComponentData() const;

	private:

		ECS::GameObjectIndex m_linkedObject = ECS::GameObjectIndex::CreateEmpty();
		ECS::ComponentType m_linkedComponent = 0;

		Views::IComponentView* m_view = nullptr;
		void* m_componentData = nullptr;

	};


	/// @brief 
	/// @tparam TComponent 
	template <typename TComponent>
	class TComponentController : public IComponentController {

	public:

		/// @brief 
		/// @param component 
		void UpdateComponent(TComponent* component) {
			Update(component);
		}

	protected:

		/// @brief 
		/// @param object 
		/// @param view 
		TComponentController(
			ECS::GameObjectIndex object,
			void* componentData,
			Views::IComponentView* view) : IComponentController(object, componentData, view) {}

		/// @brief 
		/// @return 
		TComponent* GetComponent() {
			return static_cast<TComponent*>(GetComponentData());
		}
		const TComponent* GetComponent() const {
			return static_cast<const TComponent*>(GetComponentData());
		}

	};

	template <typename _TComponentController>
	concept IsComponentController = 
		std::is_base_of_v<IComponentController, _TComponentController> &&
		requires (_TComponentController) {
			{ _TComponentController::GetComponentTypeName() };
	};

}

#define OSK_COMPONENT_EDITOR_CONTROLLER(componentName) \
constexpr static std::string_view GetComponentTypeName() { return componentName; }

#define OSK_LINK_COMPONENT_EDITOR_CONTROLLER(componentType) \
constexpr static std::string_view GetComponentTypeName() { return componentType::GetComponentTypeName(); }
