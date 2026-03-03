#include "MaterialInstance.h"

#include "Material.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "IMaterialSlot.h"

#include "Assert.h"
#include "InvalidArgumentException.h"
#include "OSKengine.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

MaterialInstance::MaterialInstance(Material* material)
	: ownerMaterial(material) {

}

void MaterialInstance::RegisterSlot(const std::string& name) {
	slots[name] = Engine::GetRenderer()->_CreateMaterialSlot(name, *GetLayout());
	slots[name]->SetDebugName(static_cast<std::string>(ownerMaterial->GetName()));
}

const IMaterialSlot* MaterialInstance::GetSlot(std::string_view name) const {
	const auto it = slots.find(name);

	OSK_ASSERT_2(
		it != slots.end(),
		InvalidArgumentException(std::format("No existe el slot {} para el material {}", name, ownerMaterial->GetName())),
		Engine::GetLogger());

	return it->second.GetPointer();
}

IMaterialSlot* MaterialInstance::GetSlot(std::string_view name) {
	auto it = slots.find(name);

	OSK_ASSERT_2(
		it != slots.end(),
		InvalidArgumentException(std::format("No existe el slot {} para el material {}", name, ownerMaterial->GetName())),
		Engine::GetLogger());

	return it->second.GetPointer();
}

Material* MaterialInstance::GetMaterial() const {
	return ownerMaterial;
}

const MaterialLayout* MaterialInstance::GetLayout() const {
	return ownerMaterial->GetLayout();
}
