#pragma once

#include "IUiContainer.h"

namespace OSK::UI {

	/// @brief Contenedor que coloca los elementos de manera horizontal y contígua.
	class OSKAPI_CALL HorizontalContainer : public IContainer {

	public:

		HorizontalContainer(const Vector2f& size);

	public:

		void ResetLayout() override;
		void EmplaceChild(IElement* child) override;

	private:

		float relativeNextPosition = 0.0f;

	};

}
