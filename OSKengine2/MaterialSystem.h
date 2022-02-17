#pragma once

#include "OSKmacros.h"

#include "LinkedList.hpp"
#include "HashMap.hpp"
#include "OwnedPtr.h"

#include <string>

namespace OSK {

	class Material;

	class OSKAPI_CALL MaterialSystem {

	public:

		Material* LoadMaterial(const std::string& path);

	private:

		LinkedList<OwnedPtr<Material>> materials;
		HashMap<std::string, Material*> materialsTable;

	};

}
