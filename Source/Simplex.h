#pragma once

#include "Vector3.hpp"
#include "DynamicArray.hpp"

#include <array>
#include <span>

#include "Gjk.h"

namespace OSK::COLLISION {

	/// @brief Grupo de vértices que trata de encerrar el origen
	/// de coordenadas (0, 0, 0).
	class OSKAPI_CALL Simplex {

	public:

		/// @brief Genera el simplex final a partir
		/// de los volúmenes @p first y @p second.
		/// @return Simplex finalizado.
		static Simplex GenerateFrom(
			const IGjkCollider& first,
			const IGjkCollider& second);

		/// @brief Añade un nuevo vértice y actualiza
		/// el estado interno.
		/// @param point Nuevo vértice.
		void AddAndUpdate(const MinkowskiSupport& point);

		/// @return Dirección en la que hay que buscar
		/// el siguiente vértice del hull GJK.
		Vector3f GetSearchDirection() const;

		/// @return Vértices del simplex.
		std::span<const MinkowskiSupport> GetVertices() const;

		/// @return Número de vértices del simplex.
		USize64 GetVerticesCount() const;

		/// @return True si el simplex contiene el origen
		/// de coordenadas (0, 0, 0).
		bool ContainsOrigin() const;

	private:

		/// @brief Añadde un nuevo punto al simplex.
		/// @param point Nuevo vértice.
		void AddPoint(const MinkowskiSupport& point);

		/// @param a Vector A.
		/// @param b Vector B.
		/// @return True si A.Dot(B) >= 0.0f.
		static bool IsSameDirection(const Vector3f& a, const Vector3f& b);


		/// @brief Actualización de estado interno en el caso
		/// de que sólamente tenga un único punto.
		void UpdatePoint();

		/// @brief Actualización de estado interno en el caso
		/// de que tenga una línea.
		void UpdateLine();

		/// @brief Actualización de estado interno en el caso
		/// de que tenga un triángulo.
		void UpdateTriangle();

		/// @brief Actualización de estado interno en el caso
		/// de que tenga una tetraedro.
		void UpdateTetrahedron();


		std::array<MinkowskiSupport, 4> m_vertices = {};
		USize64 m_verticesCount = 0;

		Vector3f m_direction = Vector3f(1.0f, 0.0f, 0.0f);
		bool m_containsOrigin = false;

	};

}
