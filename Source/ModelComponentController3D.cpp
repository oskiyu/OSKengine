#include "ModelComponentController3D.h"

#include "ModelComponentView3D.h"

#include "UiTextView.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::Editor;

Controllers::ModelComponentController3D::ModelComponentController3D(GameObjectIndex object, void* componentData, Views::IComponentView* view)
	: Controllers::TComponentController<ECS::ModelComponent3D>(object, componentData, view) {

}

void Controllers::ModelComponentController3D::Poll() {
	const ModelComponent3D& model = *GetComponent();
	Views::ModelComponentView3D* view = GetView()->As<Views::ModelComponentView3D>();

	if (m_isFirstUpdate) {
		view->SetAssetPath(model.GetModelAsset()->GetName());
		m_previousAssetPath = model.GetModelAsset()->GetName();

		view->SetCastsShadows(model.CastsShadows());
		m_castedShadows = model.CastsShadows();

		view->SetIsAnimated(model.IsAnimated());
		m_wasAnimated = model.IsAnimated();

		m_isFirstUpdate = true;
	}
	else {
		if (m_previousAssetPath != model.GetModelAsset()->GetName()) {
			view->SetAssetPath(model.GetModelAsset()->GetName());
			m_previousAssetPath = model.GetModelAsset()->GetName();
		}

		if (m_castedShadows != model.CastsShadows()) {
			view->SetCastsShadows(model.CastsShadows());
			m_castedShadows = model.CastsShadows();
		}

		if (m_wasAnimated != model.IsAnimated()) {
			view->SetIsAnimated(model.IsAnimated());
			m_wasAnimated = model.IsAnimated();
		}
	}

	view->ClearShaderPasses();
	for (const auto& name : model.GetShaderPassNames()) {
		view->AddShaderPass(name);
	}
	view->DeleteUnusedPasses();
}
