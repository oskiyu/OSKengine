#include "IMaterialSlot.h"

using namespace OSK;

IMaterialSlot::IMaterialSlot(const MaterialLayout* layout, const std::string& name)
	: name(name), layout(layout) {

}

std::string IMaterialSlot::GetName() const {
	return name;
}
