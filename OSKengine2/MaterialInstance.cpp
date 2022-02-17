#include "MaterialInstance.h"

#include "Material.h"
#include "OSKengine.h"
#include "IRenderer.h"

using namespace OSK;

MaterialInstance::MaterialInstance(const Material* material)
	: ownerMaterial(material) {

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
