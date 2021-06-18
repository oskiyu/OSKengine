#pragma once

#include "Vertex.h"
#include "Model.h"
#include "ContentManager.h"

#include "ToString.h"
#include "Heightmap.h"
#include "CollisionBox.h"
#include "MaterialInstance.h"

#include <cstdlib>

namespace OSK {

	/// <summary>
	/// Clase que representa un terreno 3D.
	/// Este terreno se construye a partir de un heightmap.
	/// </summary>
	class OSKAPI_CALL Terrain {

		friend class RenderizableScene;
		friend class PhysicsSystem;

	public:

		/// <summary>
		/// Crea la instancia del terreno.
		/// Aún no se ha generado.
		///	<Content>: ContentManager para la generación del terreno.
		/// </summary>
		/// <param name="Content">Content manager para crear el modelo 3D del terreno.</param>
		Terrain(ContentManager* Content);

		/// <summary>
		/// Destruye el terreno.
		/// </summary>
		~Terrain();

		/// <summary>
		/// Genera el terreno.
		/// </summary>
		/// <param name="path">Ruta del heightmap.</param>
		/// <param name="quadSize">Tamaño de cada pixel del terreno, en unidades del mundo..</param>
		/// <param name="maxHeight">Altura máxima del terreno.</param>
		void CreateMesh(const std::string& path, const Vector2f& quadSize, float maxHeight);

		/// <summary>
		/// Obtiene la altura del terreno en un punto específico.
		/// Devuelve 0 si el punto está fuera del terreno.
		/// </summary>
		/// <param name="point">Punto (en coordenadas del mundo 2D).</param>
		/// <returns>Altura del punto del terreno.</returns>
		float_t GetHeight(const Vector2f& point) const;

		/// <summary>
		/// Obtiene la altura de un vértice en concreto.
		/// </summary>
		/// <param name="point">Coordenadas (en el mapa) del vértice.</param>
		/// <returns>Altura del vértice.</returns>
		float_t GetVertexHeight(const Vector2i& point) const;

		/// <summary>
		/// Obtiene la CollisionBox que contiene el terreno.
		/// </summary>
		/// <returns>Collision box del terreno.</returns>
		CollisionBox GetCollisionBox() const;

		/// <summary>
		/// Tamaño del mapa, en unidades del mundo.
		/// </summary>
		Vector2f GetMapSize() const;

		/// <summary>
		/// Material del terreno.
		/// Para su renderizado.
		/// </summary>
		SharedPtr<MaterialInstance> material;

	private:

		/// <summary>
		/// Tamaño del mapa, en unidades del mundo.
		/// </summary>
		Vector2f mapSizeInWorlds{ 0.0f };

		/// <summary>
		/// Altura base del mapa.
		/// </summary>
		float_t baseHeight = 0.0f;

		/// <summary>
		/// Fricción del terreno (para el motor de físicas).
		/// </summary>
		float_t frictionCoefficient = 1.0f;

		/// <summary>
		/// Material de sombras del terreno.
		/// Para su renderizado de sombras.
		/// </summary>
		SharedPtr<MaterialInstance> shadowMaterial;

		/// <summary>
		/// Número de quads del mapa { X, Y }.
		/// </summary>
		Vector2ui mapSizeInQuads{ 0 };

		/// <summary>
		/// Tamaño de cada quad, en unidades del mundo.
		/// </summary>
		Vector2f quadSize = { 1.0f };

		/// <summary>
		/// Modelo 3D del terreno.
		/// </summary>
		ModelData* terrainModel = nullptr;

		/// <summary>
		/// CollisionBox que contiene el terreno.
		/// </summary>
		CollisionBox colisionBox{};

		/// <summary>
		/// Content manager para crear el modelo 3D del terreno.
		/// </summary>
		ContentManager* content = nullptr;

		/// <summary>
		/// Heightmap a partir del cual se ha creado el terreno.
		/// </summary>
		Heightmap map{};

		/// <summary>
		/// Multiplicador para obtener la altura que tendrá cada vértice del mapa:
		/// -MaxHeight / 255.
		/// </summary>
		float heightMultiply = -5.0f / 255;

		/// <summary>
		/// True si se ha cargado el terreno (con su heightmap).
		/// </summary>
		bool hasBeenLoaded = false;

		/// <summary>
		/// Obtiene la altura de un punto del terreno.
		/// </summary>
		/// <param name="p1">La altura de una de las esquinas del triángulo sobre el que se encuentra el punto.</param>
		/// <param name="p2">La altura de otra de las esquinas del triángulo sobre el que se encuentra el punto.</param>
		/// <param name="p3">La altura de la última de las esquinas del triángulo sobre el que se encuentra el punto.</param>
		/// <param name="pos">Posición de donde se quiere sacar su altura.</param>
		/// <returns>Altura del punto.</returns>
		float barryCentric(const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, const Vector2f& pos) const;

	};

}
