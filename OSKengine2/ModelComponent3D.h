#pragma once

#include "Component.h"
#include "OwnedPtr.h"

namespace OSK {
	namespace ASSETS {
		class Model3D;
	}
	namespace GRAPHICS {
		class MaterialInstance;
	}
}

namespace OSK::ECS {

	/// <summary>
	/// Componente para el renderizado 3D de un objeto.
	/// Incluye:
	/// - Modelo 3D.
	/// - Instancia de su material.
	/// </summary>
	class OSKAPI_CALL ModelComponent3D {

	public:

		OSK_COMPONENT("OSK::ModelComponent3D");

		~ModelComponent3D();

		void SetModel(ASSETS::Model3D* model);
		void SetMaterialInstance(OwnedPtr<GRAPHICS::MaterialInstance> materialInstance);

		ASSETS::Model3D* GetModel() const;
		GRAPHICS::MaterialInstance* GetMaterialInstance() const;

	private:

		ASSETS::Model3D* model = nullptr;
		OwnedPtr<GRAPHICS::MaterialInstance> materialInstance;

	};

}
