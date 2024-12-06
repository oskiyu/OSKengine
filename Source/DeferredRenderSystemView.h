#pragma once

#include "ISystemView.h"
#include "Vector3.hpp"

#include "DeferredRenderSystem.h"
#include "GameObject.h"
#include "DynamicArray.hpp"

namespace OSK::UI {
	class TextView;
	class CollapsibleWrapper;
}

namespace OSK::Editor::Views {

	class OSKAPI_CALL DeferredRenderSystemView : public ISystemView {

	public:

		OSK_LINK_SYSTEM_EDITOR_VIEW(ECS::DeferredRenderSystem);

		explicit DeferredRenderSystemView(const Vector2f& size);

		void SetIrradianceMapName(std::string_view name);
		void SetSpecularMapName(std::string_view name);
		void SetCameraObject(ECS::GameObjectIndex object);
		void AddShaderPass(std::string_view name);
		void AddShadowsPass(std::string_view name);
		void SetResolverPassName(std::string_view name);

	private:

		static std::string GetPassDisplayName(std::string_view name);

		void ConfigureCollapsible(OSK::UI::CollapsibleWrapper* collapsible) const;

		OSK::UI::TextView* m_irradianceMapView = nullptr;
		OSK::UI::TextView* m_specularMapView = nullptr;
		OSK::UI::TextView* m_cameraObjectView = nullptr;
		OSK::UI::CollapsibleWrapper* m_shaderPasses = nullptr;
		DynamicArray<OSK::UI::TextView*> m_shaderPassesViews{};
		OSK::UI::CollapsibleWrapper* m_shadowsPasses = nullptr;
		DynamicArray<OSK::UI::TextView*> m_shadowsPassesViews{};
		OSK::UI::TextView* m_resolvePassView = nullptr;

		Vector2f m_originalTextSize = Vector2f::Zero;

	};

}
