#pragma once

#include "Component.h"
#include "OwnedPtr.h"
#include "UniquePtr.hpp"
#include "DynamicArray.hpp"
#include "HashMap.hpp"

#include <string>

namespace OSK {
	namespace ASSETS {
		class Model3D;
		class Texture;
	}
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
		void SetModel(ASSETS::Model3D* model);

		/// @return Modelo 3D del componente.
		/// @pre Debe haberse establecido el modelo con ModelComponent3D::SetModel.
		ASSETS::Model3D* GetModel() const;


		/// @brief Establece si el modelo generará sombras.
		/// @param castShadows True si generará sombras.
		void SetCastShadows(bool castShadows) { m_castShadows = castShadows; }

		/// @return True si generará sombras.
		bool CastsShadows() const { return m_castShadows; }

	private:

		ASSETS::Model3D* m_model = nullptr;

		bool m_castShadows = true;

	};

}
