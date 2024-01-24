#include "MaterialInstance.h"

#include "Material.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "IMaterialSlot.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

MaterialInstance::MaterialInstance(Material* material)
	: ownerMaterial(material) {

}

MaterialInstance::~MaterialInstance() {

}

void MaterialInstance::RegisterSlot(const std::string& name) {
	slots[name] = Engine::GetRenderer()->_CreateMaterialSlot(name, *GetLayout()).GetPointer();
}

Material* MaterialInstance::GetMaterial() const {
	return ownerMaterial;
}

const MaterialLayout* MaterialInstance::GetLayout() const {
	return ownerMaterial->GetLayout();
}
