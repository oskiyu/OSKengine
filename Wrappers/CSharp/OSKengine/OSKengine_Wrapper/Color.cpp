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

Color^ Color::operator*(Color^ dis, float value) {
	Color^ output = gcnew Color(dis->Red, dis->Green, dis->Blue, value);
	
	return output;
}

System::String^ Color::ToString() {
	return "{ R: " + Red + "; G: " + Green + "; B: " + Blue + "; A: " + Alpha + " }";
}