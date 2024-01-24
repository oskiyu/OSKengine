#pragma once

#include "IUiContainer.h"
#include "UiAnchor.h"

namespace OSK::UI {

	/// @brief Contenedor que coloca los elementos de manera vertical y contígua.
	class OSKAPI_CALL VerticalContainer : public IContainer {

	public:

		VerticalContainer(const Vector2f& size);

	public:

		void ResetLayout() override;
		void EmplaceChild(IElement* child) override;

	private:

		float relativeNextPosition = 0.0f;

	};

}
