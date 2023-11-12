#pragma once

#include "Component.h"
#include "OwnedPtr.h"
#include "UniquePtr.hpp"
#include "DynamicArray.hpp"
#include "HashMap.hpp"
#include "AssetRef.h"
#include "Model3D.h"

#include <string>

namespace OSK {
	namespace GRAPHICS {
		class Material;
		class MaterialInstance;
		class GpuImage;
		class IGpuImageView;
		class GpuBuffer;
	}
}

namespace OSK::ECS {

	/// @brief Componente para el renderizado 3D de un objeto.
	class OSKAPI_CALL ModelComponent3D {

	public:

		OSK_COMPONENT("OSK::ModelComponent3D");

		/// @brief Establece el modelo 3D que se renderizará.
		/// @param model Modelo 3D cargado.
		/// @pre @p model no debe ser null.
		void SetModel(ASSETS::AssetRef<ASSETS::Model3D> model);

		ASSETS::Model3D* GetModel();
		const ASSETS::Model3D* GetModel() const;


		/// @brief Establece si el modelo generará sombras.
		/// @param castShadows True si generará sombras.
		void SetCastShadows(bool castShadows) { m_castShadows = castShadows; }

		/// @return True si generará sombras.
		bool CastsShadows() const { return m_castShadows; }

	private:

		ASSETS::AssetRef<ASSETS::Model3D> m_model;

		bool m_castShadows = true;

	};

}
