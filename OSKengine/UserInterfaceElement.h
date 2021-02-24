#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Vector4.hpp"
#include "AnchorEnum.h"
#include <list>

namespace OSK {

	class OSKAPI_CALL UserInterfaceElement {

	public:

		virtual ~UserInterfaceElement() = default;

		void SetSize(const Vector2ui& size) {
			area.Z = size.X;
			area.W = size.Y;
		}

		void SetPosition(const Vector2ui& pos) {
			area.X = pos.X;
			area.Y = pos.Y;
		}

		void Update(deltaTime_t deltaTime);

		//Objetivo del ancla
		Anchor AnchoredTo = Anchor::TOP_LEFT;
		//Origen del ancla
		Anchor AnchorReference = Anchor::TOP_LEFT;

		void AddChildElement(UserInterfaceElement* element) {
			element->Parent = this;
			childElements.push_back(element);
		}
		void RemoveChildElement(UserInterfaceElement* element) {
			childElements.remove(element);
		}

		bool IsVisible = true;
		float Opacity = 1.0f;
		bool IsActive = true;
		bool ShouldFillParent = false;

		UserInterfaceElement* Parent = nullptr;

	private:

		Vector4ui area;
		std::list<UserInterfaceElement*> childElements;

	};

}
