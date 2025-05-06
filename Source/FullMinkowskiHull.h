#pragma once

#include "ApiCall.h"
#include "Simplex.h"
#include "DynamicArray.hpp"

#include <array>

namespace OSK::COLLISION {

	class ConvexVolume;

	
	/// @brief Volumen 3D que ocupa toda la 
	/// diferencia de Minkowski de dos
	/// colliders.
	class OSKAPI_CALL FullMinkowskiHull {

	public:

		enum class Volume { A, B };

	public:

		/// @brief Construye el hull completo de la diferencia de Minkowksi,
		/// obteniendo el MTV.
		/// @param first Primer collider.
		/// @param second Segundo collider.
		static FullMinkowskiHull BuildFromVolumes(const ConvexVolume& first, const ConvexVolume& second);

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

		constexpr static float MinDistanceThreshold = 1.0e-10f;

		
	private:

		DynamicArray<MinkowskiSupport> m_vertices;
		DynamicArray<Face> m_faces;

		Vector3f m_mtv = Vector3f::Zero;
		Volume m_referenceVolume = Volume::A;

		bool m_surpassesMinDepth = true;

	};

}
