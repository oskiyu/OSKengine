#pragma once

#include "UiElement.h"

namespace OSK {

	/// <summary>
	/// Permite que el elemento dado pueda ser arrastrado con el ratón.
	/// </summary>
	void uiSetDraggableFunctionality(UiElement* element);

	/// <summary>
	/// Estructura interna para determinar el cambio de valor
	/// de una variable en un checkbox.
	/// </summary>
	struct UiCheckBox {

		/// <summary>
		/// Función que se ejecuta.
		/// Devuelve el nuevo valor.
		/// </summary>
		std::function<bool()> change = []() { return true; };

		/// <summary>
		/// Tipo de funcionalidad.
		/// </summary>
		enum class Type {
			FUNCTION,
			INT,
			BOOL
		} type;

		/// <summary>
		/// Puntero a la variable editable.
		/// </summary>
		union {
			int* asInt;
			bool* asBool;
		} data;
	};

	/// <summary>
	/// Permite que un elemento funcione como un checkbox.
	/// </summary>
	/// <param name="element">Elemento.</param>
	/// <param name="checkbox">Comportamiento.</param>
	/// <param name="trueColor">Color del sprite, si el valor es true.</param>
	/// <param name="falseColor">Color del sprite, si el valor es false.</param>
	void uiSetCheckboxFunctionality(UiElement* element, UiCheckBox checkbox, OSK::Color trueColor, OSK::Color falseColor);

	/// <summary>
	/// Permite que un elemento funcione como un checkbox.
	/// </summary>
	/// <param name="element">Elemento.</param>
	/// <param name="dataToChange">Comportamiento.</param>
	/// <param name="trueColor">Color del sprite, si el valor es true.</param>
	/// <param name="falseColor">Color del sprite, si el valor es false.</param>
	void uiSetCheckboxFunctionality(UiElement* element, bool* dataToChange, OSK::Color trueColor, OSK::Color falseColor);

	/// <summary>
	/// Permite que un elemento funcione como un checkbox.
	/// </summary>
	/// <param name="element">Elemento.</param>
	/// <param name="dataToChange">Comportamiento.</param>
	/// <param name="trueColor">Color del sprite, si el valor es true.</param>
	/// <param name="falseColor">Color del sprite, si el valor es false.</param>
	void uiSetCheckboxFunctionality(UiElement* element, int* dataToChange, OSK::Color trueColor, OSK::Color falseColor);

	/// <summary>
	/// Permite que un elemento funcione como un checkbox.
	/// </summary>
	/// <param name="element">Elemento.</param>
	/// <param name="change">Comportamiento.</param>
	/// <param name="trueColor">Color del sprite, si el valor es true.</param>
	/// <param name="falseColor">Color del sprite, si el valor es false.</param>
	void uiSetCheckboxFunctionality(UiElement* element, std::function<bool()> change, OSK::Color trueColor, OSK::Color falseColor);

	/// <summary>
	/// Permite que el elemento funcione como un slider (horizontal).
	/// </summary>
	/// <param name="element">Elemento.</param>
	/// <param name="valueToChange">Puntero al float que contendrá el valor del slider.</param>
	/// <param name="min">Valor mínimo.</param>
	/// <param name="max">Valor máximo.</param>
	void uiSetSliderFunctionality(UiElement* element, float* valueToChange, float min, float max);
	
	/// <summary>
	/// Permite que el elemento funcione como un dropdown.
	/// </summary>
	/// <param name="element">Elemento.</param>
	/// <param name="toParent">El dropdown es el padre.</param>
	void uiSetDropdownFuncionality(UiElement* element, bool toParent);

}