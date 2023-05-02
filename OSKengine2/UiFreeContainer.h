#pragma once

#include "IUiContainer.h"

namespace OSK::UI {

	/// @brief Contenedor en el que los hijos se colocan en
	/// un lado, una esquina o centro, dependiendo de su Anchor.
	/// 
	/// No puede haber m�s de 1 elemento en cada posici�n, ya que
	/// se solapar�n.
	class OSKAPI_CALL FreeContainer : public IContainer {

	public:

		FreeContainer(const Vector2f& size);
		
	public:

		void EmplaceChild(IElement* child) override;
		void ResetLayout() override;

	};

}
