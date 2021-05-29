#include "UiFunctionality.h"

using namespace OSK;

void OSK::uiSetDraggableFunctionality(UiElement* element) {
	element->onClick = [](UiElement* sender, const OSK::Vector2f& mousePos) {
		sender->previousPos = mousePos;
	};
	element->onHold = [](UiElement* sender, const OSK::Vector2f& mousePos) {
		OSK::Vector2f diff = mousePos - sender->previousPos;

		if (sender->dragMoveParent)
			sender->parent->AddPosition(diff);
		else
			sender->AddPosition(diff);

		sender->previousPos = mousePos;
	};
}

void OSK::uiSetCheckboxFunctionality(UiElement* element, UiCheckBox checkbox, OSK::Color trueColor, OSK::Color falseColor) {
	switch (checkbox.type) {

	case UiCheckBox::Type::BOOL:
	case UiCheckBox::Type::INT:
		element->onClick = [checkbox, trueColor, falseColor](UiElement* sender, const OSK::Vector2f& mousePos) {
			*checkbox.data.asBool = !*checkbox.data.asBool;

			bool finalBool = *checkbox.data.asBool;

			if (finalBool)
				sender->originalSpireColor = trueColor;
			else
				sender->originalSpireColor = falseColor;
		};
		{
			bool finalBool = *checkbox.data.asBool;

			if (finalBool)
				element->originalSpireColor = trueColor;
			else
				element->originalSpireColor = falseColor;
		}
		break;

	case UiCheckBox::Type::FUNCTION:
		element->onClick = [checkbox, trueColor, falseColor](UiElement* sender, const OSK::Vector2f& mousePos) {
			bool finalBool = checkbox.change();

			if (finalBool)
				sender->originalSpireColor = trueColor;
			else
				sender->originalSpireColor = falseColor;
		};

		checkbox.change();
		if (checkbox.change())
			element->originalSpireColor = trueColor;
		else
			element->originalSpireColor = falseColor;

		break;
	}
}

void OSK::uiSetCheckboxFunctionality(UiElement* element, bool* dataToChange, OSK::Color trueColor, OSK::Color falseColor) {
	UiCheckBox box;
	box.data.asBool = dataToChange;
	box.type = UiCheckBox::Type::BOOL;

	uiSetCheckboxFunctionality(element, box, trueColor, falseColor);
}
void OSK::uiSetCheckboxFunctionality(UiElement* element, int* dataToChange, OSK::Color trueColor, OSK::Color falseColor) {
	UiCheckBox box;
	box.data.asInt = dataToChange;
	box.type = UiCheckBox::Type::INT;

	uiSetCheckboxFunctionality(element, box, trueColor, falseColor);
}
void OSK::uiSetCheckboxFunctionality(UiElement* element, std::function<bool()> change, OSK::Color trueColor, OSK::Color falseColor) {
	UiCheckBox box;
	box.change = change;
	box.type = UiCheckBox::Type::FUNCTION;

	uiSetCheckboxFunctionality(element, box, trueColor, falseColor);
}

void OSK::uiSetSliderFunctionality(UiElement* element, float* valueToChange, float min, float max) {
	element->onHold = [valueToChange, min, max](UiElement* sender, const OSK::Vector2f& mousePos) {
		sender->sliderValue = mousePos.X - (sender->GetPosition().X + sender->padding.X);

		float value = sender->sliderValue / (sender->GetSize().Y - sender->padding.Y * 2);
		if (value > 1.0f)
			value = 1.0f;
		if (value < 0.0f)
			value = 0.0f;

		value = value * (max - min) + min;
	};
}

void OSK::uiSetDropdownFuncionality(UiElement* element, bool toParent) {
	element->onClick = [toParent](UiElement* sender, const OSK::Vector2f& mousePos) {
		if (sender->parent) {
			sender->parent->isCompacted = !sender->parent->isCompacted;

			for (auto child : sender->parent->children)
				if (child != sender)
					child->isActive = !sender->parent->isCompacted;

			sender->parent->Shrink();
		}
	};
}