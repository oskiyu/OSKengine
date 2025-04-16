#pragma once

#include "Component.h"
#include "UniquePtr.hpp"
#include "DynamicArray.hpp"
#include "HashMap.hpp"
#include "AssetRef.h"
#include "Model3D.h"

#include <string>

#include "Serializer.h"
#include "HashMap.hpp"
#include <set>


namespace OSK::GRAPHICS {
	class Material;
	class MaterialInstance;
	class GpuImage;
	class IGpuImageView;
	class GpuBuffer;
}

namespace OSK::ECS {

	/// @brief Componente para el renderizado 3D de un objeto.
	class OSKAPI_CALL ModelComponent3D {

	public:

		OSK_SERIALIZABLE_COMPONENT();

	public:

		OSK_COMPONENT("OSK::ModelComponent3D");

		/// @brief Establece el modelo 3D que se renderizará.
		/// @param model Modelo 3D cargado.
		/// @pre @p model no debe ser null.
		void SetModel(ASSETS::AssetRef<ASSETS::Model3D> model);

		ASSETS::Model3D* GetModelAsset();
		const ASSETS::Model3D* GetModelAsset() const;

		GRAPHICS::GpuModel3D* GetModel();
		const GRAPHICS::GpuModel3D* GetModel() const;


		/// @brief Establece si el modelo generará sombras.
		/// @param castShadows True si generará sombras.
		void SetCastShadows(bool castShadows) { m_castShadows = castShadows; }

		/// @return True si generará sombras.
		bool CastsShadows() const { return m_castShadows; }

		GRAPHICS::Animator& GetAnimator();
		const GRAPHICS::Animator& GetAnimator() const;

		bool IsAnimated() const;

		void AddShaderPassName(const std::string& name);
		void RemoveShaderPassName(const std::string& name);
		const std::unordered_set<std::string, StringHasher, std::equal_to<>>& GetShaderPassNames() const;

	private:

		ASSETS::AssetRef<ASSETS::Model3D> m_model;

		GRAPHICS::Animator m_animator{};

		bool m_castShadows = true;
		std::unordered_set<std::string, StringHasher, std::equal_to<>> m_shaderNames;

	};

}

OSK_COMPONENT_SERIALIZATION(OSK::ECS::ModelComponent3D);
