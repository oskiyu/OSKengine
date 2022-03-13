#include "MaterialInstance.h"

#include "Material.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "IMaterialSlot.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

MaterialInstance::MaterialInstance(const Material* material)
	: ownerMaterial(material) {

}

MaterialInstance::~MaterialInstance() {
	for (auto& i : slots)
		delete i.second.GetPointer();
}

IMaterialSlot* MaterialInstance::GetSlot(const std::string& name) {
	return slots.Get(name).GetPointer();
}

void MaterialInstance::RegisterSlot(const std::string& name) {
	slots.Insert(name, Engine::GetRenderer()->_CreateMaterialSlot(name, GetLayout()));
}

const Material* MaterialInstance::GetMaterial() const {
	return ownerMaterial;
}

const MaterialLayout* MaterialInstance::GetLayout() const {
	return ownerMaterial->GetLayout();
}
