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
		/// @pre @p simplex debe estar completo (4 v�rtices) y debe
		/// contener el origen (0, 0, 0).
		static Polytope FromSimplex(const Simplex& simplex);

		/// @brief Expande el polytope hasta encontrar el MTV.
		/// @param first Primer volumen que gener� el simplex.
		/// @param second Segundo volumen que gener� el simplex.
		void Expand(const ConvexVolume& first, const ConvexVolume& second);
#endif // OSK_EPA_IMPL

		/// @brief Construye el hull completo de la diferencia de Minkowksi,
		/// obteniendo el MTV.
		/// @param first Primer collider.
		/// @param second Segundo collider.
		void BuildFullHull(const ConvexVolume& first, const ConvexVolume& second);


		/// @return True si la profundidad del MTV es lo suficientemente
		/// grande como para considerar que hay colisi�n.
		bool SurpasesMinDepth() const;


		/// @return Vector m�nimo de translaci�n.
		Vector3f GetMtv() const;

		/// @return MTV normalizado.
		Vector3f GetNormalizedMtv() const;


		/// @return IDs de los v�rtices del primer volumen que
		/// gener� el simplex.
		std::array<UIndex64, 3> GetFirstFaceVerticesIds() const;

		/// @return IDs de los v�rtices del segundo volumen que
		/// gener� el simplex.
		std::array<UIndex64, 3> GetSecondFaceVerticesIds() const;

		Volume GetReferenceId() const;

	private:

		/// @brief �ndices de una cara.
		using Face = std::array<UIndex64, 3>;

#ifdef OSK_EPA_IMPL

		/// @brief Representa una arista del polytope.
		struct Edge {

			/// @brief �ndice del primer v�rtice.
			UIndex64 a;

			/// @brief �ndice del segundo v�rtice.
			UIndex64 b;

			bool operator==(const Edge&) const = default;
		};
#endif

		constexpr static float MinDistanceThreshold = 1.0e-10f;

		
	private:

#ifdef OSK_EPA_IMPL
		Polytope(const Simplex& simplex);

		/// @brief Procesa la lista @p edges, a�adiendo
		/// la arista formada por @p vertexA y @p vertexB
		/// si no estaba previamente en la lista, y elimin�ndola
		/// de la lista si ya estaba a�adida.
		/// @param edges Lista de aristas.
		/// @param vertexA Primer v�rtice de la arista.
		/// @param vertexB Segundo v�rtice de la arista.
		static void AddUniqueEdge(
			DynamicArray<Edge>* edges,
			UIndex64 vertexA,
			UIndex64 vertexB);


		void Update();


		/// @brief Construye nuevas caras a partir de las aristas
		/// y v�rtices indicados.
		/// @param edges Aristas a partir de las que se
		/// construir�n las nuevas caras.
		/// @param vertices Lista de los v�rtices que ser� actualizada.
		/// @param newVertex Nuevo v�rtice a a�adir.
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
