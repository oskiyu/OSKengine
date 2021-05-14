#pragma once

#include "ECS.h"
#include "AnimatedModel.h"
#include "ContentManager.h"

namespace OSK {

	/// <summary>
	/// Componente que permite a una entidad tener modelos 3D.
	/// </summary>
	class ModelComponent : public Component {

	public:

		/// <summary>
		/// Añade un modelo 3D al componente.
		/// </summary>
		/// <param name="path">Ruta del modelo (con extensión).</param>
		/// <param name="content">Content manager con el que se va a cargar el modelo.</param>
		void AddModel(const std::string& path, ContentManager* content);

		/// <summary>
		/// Añade un modelo 3D animado al componente.
		/// </summary>
		/// <param name="path">Ruta del modelo (con extensión).</param>
		/// <param name="content">Content manager con el que se va a cargar el modelo.</param>
		void AddAnimatedModel(const std::string& path, ContentManager* content);

		/// <summary>
		/// Enlaza los modelos 3D al transform dado.
		/// Usado para enlazarlos al transform de la entidad.
		/// </summary>
		/// <param name="transform">Transform.</param>
		void Link(Transform* transform);

		/// <summary>
		/// Modelos 3D.
		/// </summary>
		std::vector<Model>& GetStaticMeshes() {
			return staticMeshes;
		}

		/// <summary>
		/// Modelos 3D animados.
		/// </summary>
		std::vector<AnimatedModel>& GetAnimatedModels() {
			return animatedModels;
		}

	private:

		/// <summary>
		/// Modelos 3D.
		/// </summary>
		std::vector<Model> staticMeshes{};

		/// <summary>
		/// Modelos 3D animados.
		/// </summary>
		std::vector<AnimatedModel> animatedModels{};

	};

}
