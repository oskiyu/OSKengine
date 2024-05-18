#include "ShaderPassFactory.h"

#include "Assert.h"
#include "InvalidArgumentException.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void ShaderPassFactory::RegisterShaderPass(const std::string& name, FactoryMethod method) {
	std::lock_guard lock(m_mutex.mutex);
	m_methods[name] = method;
}

bool ShaderPassFactory::ContainsShaderPass(std::string_view name) const {
	std::lock_guard lock(m_mutex.mutex);
	return m_methods.contains(name);
}

OwnedPtr<IShaderPass> ShaderPassFactory::CreatePass(std::string_view name) const {
	OSK_ASSERT(
		ContainsShaderPass(name),
		InvalidArgumentException(std::format("El pase {} no ha sido registrado.", name)));

	std::lock_guard lock(m_mutex.mutex);
	return m_methods.find(name)->second();
}
