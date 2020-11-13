#include "Color.h"

using namespace OSKengine;

Color::Color() : Wrapper(new OSK::Color()) {

}

Color::Color(float r, float g, float b) : Wrapper(new OSK::Color(r, g, b)) {

}

Color::Color(float r, float g, float b, float a) : Wrapper(new OSK::Color(r, g, b, a)) {

}

Color::Color(float value) : Wrapper(new OSK::Color(value)) {

}

Color^ Color::operator*(float value) {
	Color^ output = gcnew Color();

	OSK::Color* color = new OSK::Color();
	
	*color = output->instance->operator*(value);

	delete output->instance;
	output->instance = color;

	return output;
}