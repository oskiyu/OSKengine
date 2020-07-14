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
	//Interfaz para elementos de UI que soportan input por rat�n.
	class OSKAPI_CALL IUserInterfaceMouseInput {

	public:
		
		//Destructor de la interfaz.
		virtual ~IUserInterfaceMouseInput() = default;


		//Actualiza y comprueba el input, y ejecuta las funciones convenientes.
		virtual void Update(const MouseState& mouseState);


		//Funci�n que se ejecuta cuando se hace click izquierdo en el elemento.
		std::function<void()> OnLeftClick = [] {};


		//Funci�n que se ejecuta cuando se hace click derecho en el elemento.
		std::function<void()> OnRightClick = [] {};


		//Funci�n que se ejecuta cuando el rat�n pasa por encima del elemento.
		std::function<void()> OnMouseHover = [] {};


		//Funci�n que se ejecuta cuando el rat�n deja de estar encima del elemento.
		std::function<void()> OnMouseUnhover = [] {};


		//Funci�n que se ejecuta cuando se gira la rueda del rat�n encima del elemento.
		std::function<void(const float_t & val)> OnWheelChange = [](const float_t& val) {};


		//Funci�n que se ejecuta cuando se hace click con la rueda del rat�n en el elemento.
		std::function<void()> OnWheelClick = [] {};


		//Retorna 'true' si el rat�n est� encima de este elemento.
		inline bool IsBeingHovered() const;


		//Retorna 'true' si el elemento est� siendo clickado con el bot�n izquierdo.
		inline bool IsBeingLeftClicked() const;


		//Retorna 'true' si el elemento est� siendo clickado con el bot�n derecho.
		inline bool IsBeingRightClicked() const;


		//Retorna 'true' si el elemento est� siendo clickado con la rueda.
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