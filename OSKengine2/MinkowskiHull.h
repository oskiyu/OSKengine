#pragma once

#include "Simplex.h"

#include <array>

namespace OSK::COLLISION {

	class ConvexVolume;

	
	/// @brief Volumen 3D que trata de expandirse para
	/// ocupar toda la diferencia de Minkowski de dos
	/// colliders.
	class OSKAPI_CALL MinkowskiHull {

	public:

		enum class Volume { A, B };

	public:

		/// @brief Construye el hull completo de la diferencia de Minkowksi,
		/// obteniendo el MTV.
		/// @param first Primer collider.
		/// @param second Segundo collider.
		static MinkowskiHull BuildFromVolumes(const ConvexVolume& first, const ConvexVolume& second);

#ifdef OSK_EPA_IMPL
		/// @brief Inicializa el polytope a partir del simplex @p simplex.
		/// @return Polytope iniciado.
		/// @pre @p simplex debe estar completo (4 vértices) y debe
		/// contener el origen (0, 0, 0).
		static Polytope FromSimplex(const Simplex& simplex);

		/// @brief Expande el polytope hasta encontrar el MTV.
		/// @param first Primer volumen que generó el simplex.
		/// @param second Segundo volumen que generó el simplex.
		void Expand(const ConvexVolume& first, const ConvexVolume& second);
#endif // OSK_EPA_IMPL

		/// @brief Construye el hull completo de la diferencia de Minkowksi,
		/// obteniendo el MTV.
		/// @param first Primer collider.
		/// @param second Segundo collider.
		void BuildFullHull(const ConvexVolume& first, const ConvexVolume& second);


		/// @return True si la profundidad del MTV es lo suficientemente
		/// grande como para considerar que hay colisión.
		bool SurpasesMinDepth() const;


		/// @return Vector mínimo de translación.
		Vector3f GetMtv() const;

		/// @return MTV normalizado.
		Vector3f GetNormalizedMtv() const;


		/// @return IDs de los vértices del primer volumen que
		/// generó el simplex.
		std::array<UIndex64, 3> GetFirstFaceVerticesIds() const;

		/// @return IDs de los vértices del segundo volumen que
		/// generó el simplex.
		std::array<UIndex64, 3> GetSecondFaceVerticesIds() const;

		Volume GetReferenceId() const;

	private:

		/// @brief Índices de una cara.
		using Face = std::array<UIndex64, 3>;

#ifdef OSK_EPA_IMPL

		/// @brief Representa una arista del polytope.
		struct Edge {

			/// @brief Índice del primer vértice.
			UIndex64 a;

			/// @brief Índice del segundo vértice.
			UIndex64 b;

			bool operator==(const Edge&) const = default;
		};
#endif

		constexpr static float MinDistanceThreshold = 1.0e-10f;

		
	private:

#ifdef OSK_EPA_IMPL
		Polytope(const Simplex& simplex);

		/// @brief Procesa la lista @p edges, añadiendo
		/// la arista formada por @p vertexA y @p vertexB
		/// si no estaba previamente en la lista, y eliminándola
		/// de la lista si ya estaba añadida.
		/// @param edges Lista de aristas.
		/// @param vertexA Primer vértice de la arista.
		/// @param vertexB Segundo vértice de la arista.
		static void AddUniqueEdge(
			DynamicArray<Edge>* edges,
			UIndex64 vertexA,
			UIndex64 vertexB);


		void Update();


		/// @brief Construye nuevas caras a partir de las aristas
		/// y vértices indicados.
		/// @param edges Aristas a partir de las que se
		/// construirán las nuevas caras.
		/// @param vertices Lista de los vértices que será actualizada.
		/// @param newVertex Nuevo vértice a añadir.
		/// @return Nuevas caras.
		static DynamicArray<Face> BuildNewFaces(
			const DynamicArray<Edge>& edges,
			DynamicArray<MinkowskiSupport>* vertices,
			const MinkowskiSupport& newVertex);

		Vector3f GetSupport(const Vector3f& normalizedDirection) const;

		DynamicArray<Vector3d> m_normals;
		DynamicArray<double> m_distances;
		UIndex64 m_minDistanceIndex = 0;

		bool m_earlyExit = false;
#endif

		DynamicArray<MinkowskiSupport> m_vertices;
		DynamicArray<Face> m_faces;

		Vector3f m_mtv = Vector3f::Zero;
		Volume m_referenceVolume = Volume::A;

		bool m_surpassesMinDepth = true;

	};

}
