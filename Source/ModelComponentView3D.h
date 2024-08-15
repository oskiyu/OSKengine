#pragma once

#include "IComponentView.h"
#include "ModelComponent3D.h"

#include <string>

namespace OSK::UI {
	class TextView;
}

namespace OSK::Editor::Views {

	class OSKAPI_CALL ModelComponentView3D : public IComponentView {

	public:

		OSK_LINK_COMPONENT_EDITOR_VIEW(ECS::ModelComponent3D);

		explicit ModelComponentView3D(const Vector2f& size);

		void SetAssetPath(const std::string& path);
		void SetCastsShadows(bool castsShadows);
		void SetIsAnimated(bool isAnimated);

		void ClearShaderPasses();
		void AddShaderPass(const std::string& name);
		void DeleteUnusedPasses();

	private:

		static std::string GetPassDisplayName(const std::string& name);

		OSK::UI::TextView* m_assetPathView = nullptr;
		OSK::UI::TextView* m_castsShadowsView = nullptr;
		OSK::UI::TextView* m_isAnimatedView = nullptr;
		OSK::UI::TextView* m_shaderPassesTitle = nullptr;
		DynamicArray<OSK::UI::TextView*> m_shaderPassesViews{};
		DynamicArray<OSK::UI::TextView*> m_refreshedPassesViews{};

		Vector2f m_originalTextSize = Vector2f::Zero;

	};

}
