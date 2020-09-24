#pragma once

#include "Vertex.h"
#include "Model.h"
#include "ContentManager.h"

#include "ToString.h"
#include "Heightmap.h"
#include "CollisionBox.h"

#include <cstdlib>


namespace OSK {

	//Calse que representa un terreno 3D.
	//Este terreno se construye a partir de un heightmap.
	class Terrain {

		friend class RenderizableScene;

	public:

		//Crea la instancia del terreno.
		//A�n no se ha generado.
		//	<Content>: ContentManager para la generaci�n del terreno.
		Terrain(ContentManager* Content);

		//Destruye el terreno.
		~Terrain();

		//Genera el terreno.
		//	<path>: ruta del heightmap.
		//	<quadSize>: tama�o de cada cuadrado del terreno.
		//	<maxHeight>: m�xima altura del terreno.
		void CreateMesh(const std::string& path, const Vector2f& quadSize, const float_t& maxHeight);

		//Obtiene la altura del terreno en un punto espec�fico.
		//	<point>: punto (en coordenadas del mundo).
		float_t GetHeight(const Vector2f& point) const;

		//Obtiene la altura de un v�rtice en concreto.
		//	<point>: coordenadas (en el mapa) del v�rtice.
		float_t GetVertexHeight(const Vector2i& point) const;

		//Obtiene la CollisionBox que contiene el terreno.
		CollisionBox GetCollisionBox() const;

		//Modelo 3D del terreno.
		ModelData* terrainModel = nullptr;

	private:

		Vector2ui MapSizeInQuads{ 0 };
		Vector2f QuadSize = { 1.0f };
		Vector2f MapSizeInWorlds{ 0.0f };
		float_t BaseHeight = 0.0f;

		CollisionBox Box{};

		ContentManager* Content = nullptr;

		Heightmap Map{};

		float HeightMultiply = -5.0f / 255;

		bool HasBeenLoaded = false;

		const float barryCentric(const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, const Vector2f& pos) const;

	};

}
