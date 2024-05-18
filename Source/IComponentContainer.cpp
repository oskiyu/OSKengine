#include "IComponentContainer.h"

using namespace OSK;
using namespace OSK::ECS;

void IComponentContainer::SetComponentType(ComponentType type) {
	m_componentType = type;
}

ComponentType IComponentContainer::GetComponentType() const {
	return m_componentType;
}
