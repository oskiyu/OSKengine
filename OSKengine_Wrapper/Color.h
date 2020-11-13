#pragma once

#include "Wrapper.h"

#include <OSKengine/Color.h>

namespace OSKengine {

	public ref class Color : public Wrapper<OSK::Color> {

		Color();
		Color(float r, float g, float b);
		Color(float r, float g, float b, float a); 
		Color(float value);

		Color^ operator*(float value);

		property float Red {
	public:
		float get() {
			return instance->Red;
		}
		void set(float val) {
			instance->Red = val;
		}
		}
		property float Green {
	public:
		float get() {
			return instance->Green;
		}
		void set(float val) {
			instance->Green = val;
		}
		}
		property float Blue {
	public:
		float get() {
			return instance->Blue;
		}
		void set(float val) {
			instance->Blue = val;
		}
		}
		property float Alpha {
	public:
		float get() {
			return instance->Alpha;
		}
		void set(float val) {
			instance->Alpha = val;
		}
		}

	};

}