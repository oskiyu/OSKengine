#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "BaseUIElement.h"
#include "MouseState.h"
#include "KeyboardState.h"

namespace OSK::UI {

	OSK_INFO_INTERNAL
	//Interfaz para elementos de UI que soportan input por ratón.
	class OSKAPI_CALL IUserInterfaceMouseInput {

	public:
		
		//Destructor de la interfaz.
		virtual ~IUserInterfaceMouseInput() = default;


		//Actualiza y comprueba el input, y ejecuta las funciones convenientes.
		virtual void Update(const MouseState& mouseState);


		//Función que se ejecuta cuando se hace click izquierdo en el elemento.
		std::function<void()> OnLeftClick = [] {};


		//Función que se ejecuta cuando se hace click derecho en el elemento.
		std::function<void()> OnRightClick = [] {};


		//Función que se ejecuta cuando el ratón pasa por encima del elemento.
		std::function<void()> OnMouseHover = [] {};


		//Función que se ejecuta cuando el ratón deja de estar encima del elemento.
		std::function<void()> OnMouseUnhover = [] {};


		//Función que se ejecuta cuando se gira la rueda del ratón encima del elemento.
		std::function<void(const float_t & val)> OnWheelChange = [](const float_t& val) {};


		//Función que se ejecuta cuando se hace click con la rueda del ratón en el elemento.
		std::function<void()> OnWheelClick = [] {};


		//Retorna 'true' si el ratón está encima de este elemento.
		inline bool IsBeingHovered() const;


		//Retorna 'true' si el elemento está siendo clickado con el botón izquierdo.
		inline bool IsBeingLeftClicked() const;


		//Retorna 'true' si el elemento está siendo clickado con el botón derecho.
		inline bool IsBeingRightClicked() const;


		//Retorna 'true' si el elemento está siendo clickado con la rueda.
		inline bool IsBeingWheelClicked() const;

	protected:

		IUserInterfaceMouseInput(BaseUIElement* base);

		virtual void leftClick();

		virtual void rightClick();

		virtual void mouseHover();

		virtual void mouseUnhover();

		virtual void wheelChange(const float_t& val);

		virtual void wheelClick();
		
		BaseUIElement* base;

		bool isBeingHovered = false;

		bool isBeingLeftClicked = false;
		
		bool isBeingRightClicked = false;

		bool isBeingWheelClicked = false;

	};

}