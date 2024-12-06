#pragma once

#include "ApiCall.h"

#include "ISystem.h"
#include "InvalidArgumentException.h"
#include <type_traits>

namespace OSK::Editor::Views {
	class ISystemView;
}

namespace OSK::Editor::Controllers {

	/// @brief Elemento que conecta los datos de un sistema
	/// con un elemento de interfaz de usuario del editor correspondiente
	/// a ese sistema.
	class OSKAPI_CALL ISystemController {

	public:

		virtual ~ISystemController() = default;

		/// @brief Actualiza la interfaz de usuario
		/// a partir de los datos del sistema.
		virtual void Poll() = 0;

		virtual std::string_view _GetSystemName() const = 0;

	protected:

		explicit ISystemController(
			ECS::ISystem* systemData,
			Views::ISystemView* view);

		Views::ISystemView* GetView();
		const Views::ISystemView* GetView() const;

		ECS::ISystem* GetSystemData();
		const ECS::ISystem* GetSystemData() const;

	private:

		ECS::ISystem* m_system = nullptr;
		Views::ISystemView* m_view = nullptr;

	};


	/// @brief 
	/// @tparam TComponent 
	template <typename TSystem>
		requires ECS::IsEcsSystem<TSystem>
	class TSystemController : public ISystemController {

	protected:

		/// @brief 
		/// @param object 
		/// @param view 
		explicit TSystemController(
			ECS::ISystem* ecsSystem,
			Views::ISystemView* view) : ISystemController(ecsSystem, view) {
			OSK_ASSERT(
				ecsSystem->Is<TSystem>(),
				InvalidArgumentException("El sistema proporcionado no es del tipo correcto."));
		}

		TSystem* GetSystem() {
			return static_cast<TSystem*>(GetSystemData());
		}
		const TSystem* GetComponent() const {
			return static_cast<const TSystem*>(GetSystemData());
		}

	};

	template <typename _TSystemController>
	concept IsSystemController =
		std::is_base_of_v<ISystemController, _TSystemController>&&
		requires (_TSystemController) {
			{ _TSystemController::GetSystemName() };
	};

}

#define OSK_SYSTEM_EDITOR_CONTROLLER(systemName) \
constexpr static std::string_view GetSystemName() { return systemName; } \
std::string_view _GetSystemName() const override { return systemName; }

#define OSK_LINK_SYSTEM_EDITOR_CONTROLLER(systemType) \
constexpr static std::string_view GetSystemName() { return systemType::GetSystemName(); } \
std::string_view _GetSystemName() const override { return systemType::GetSystemName(); }
