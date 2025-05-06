#pragma once

#include "ApiCall.h"
#include "DynamicArray.hpp"
#include "Gjk.h"

#include <array>


namespace OSK::COLLISION {

	class Simplex;


	/// @brief Hull de Minkowski parcial,
	/// que parte del s�mplex obtenido mediante
	/// GJK y lo expande iterativamente hasta 
	/// encontrar el MTV y el punto de contacto.
	class OSKAPI_CALL EpaMinkowskiHull {

	public:

		/// @brief Genera el Minkowski hull
		/// parcial a partir del s�mplex y de
		/// los colliders indicados.
		/// @param startingSimplex S�mplex
		/// obtenido de GJK a partir de los
		/// colliders @p first y @p second.
		/// @param first Primer collider.
		/// @param second Segundo collider.
		/// @return Minkowski hull parcial.
		static EpaMinkowskiHull From(
			const Simplex& startingSimplex,
			const IGjkCollider& first,
			const IGjkCollider& second);

		/// @return True si el MTV supera la
		/// distancia m�nima para poder considerarse
		/// como colisi�n v�lida.
		bool SurpasesMinDepth() const;

		/// @return Minimum translation vector.
		Vector3f GetMtv() const;

		/// @return Punto de contacto en espacio del mundo.
		Vector3f GetContactPoint() const;

	private:
		
		/// @brief Representa una arista del polytope.
		struct Edge {

			/// @brief �ndice del primer v�rtice.
			UIndex64 a;

			/// @brief �ndice del segundo v�rtice.
			UIndex64 b;

			bool operator==(const Edge&) const;

		};

		/// @brief Las caras son tri�ngulos.
		using Face = std::array<UIndex64, 3>;

		/// @brief Inicializa el hull al valor
		/// indicado por el s�mplex.
		/// @param startingSimplex S�mplex
		/// obtenido mediante GJK.
		explicit EpaMinkowskiHull(const Simplex& startingSimplex);

		/// @brief Por cada cara, actualiza:
		/// - Vector normal.
		/// - Distancia.
		void UpdateNormalsAndDistances();

		/// @brief Expande el polytope.
		/// @param first Primer collider.
		/// @param second Segundo collider.
		void Expand(
			const IGjkCollider& first, 
			const IGjkCollider& second);

		/// @brief Trata de a�adir una arista �nica.
		/// 
		/// En caso de que ya existiese, no se a�ade
		/// (y se elimina dicha arista).
		/// 
		/// @param edges Lista de aristas.
		/// @param vertexA Primer v�rtice.
		/// @param vertexB Segundo v�rtice.
		static void AddUniqueEdge(
			DynamicArray<Edge>* edges, 
			UIndex64 vertexA, 
			UIndex64 vertexB);

		/// @brief Construye y devuelve una lista con las
		/// nuevas caras generadas al a�adir el v�rtice.
		/// 
		/// @param edges Lista de aristas que se usar�n
		/// para construir las caras.
		/// @param vertices Lista de v�rtices (se a�adir�
		/// el nuevo v�rtice).
		/// @param newVertex Nuevo v�rtice.
		/// @return Lista con las nuevas caras.
		/// 
		/// @post @p vertices tendr� un v�rtice m�s.
		static DynamicArray<Face> BuildNewFaces(
			const DynamicArray<Edge>& edges, 
			DynamicArray<MinkowskiSupport>* vertices, 
			const MinkowskiSupport& newVertex);

		/// @param direction Direcci�n a buscar.
		/// @return Soporte de este hull respecto
		/// a la direcci�n @p direction.
		Vector3f GetSupport(const Vector3f& direction) const;

		DynamicArray<MinkowskiSupport> m_vertices;
		DynamicArray<Face> m_faces;
		DynamicArray<Vector3f> m_faceNormals;
		DynamicArray<float> m_distances;

		bool m_surpassesMinDepth = true;
		bool m_earlyExit = false;
		UIndex64 m_minDistanceIndex = 0;

		constexpr static float MinDistanceThreshold = 1.0e-5f;

	};

}
