#pragma once

#include "Vector3.hpp"
#include "DynamicArray.hpp"

#include <array>
#include <span>

#include "Gjk.h"

namespace OSK::COLLISION {

	/// @brief Grupo de v�rtices que trata de encerrar el origen
	/// de coordenadas (0, 0, 0).
	class OSKAPI_CALL Simplex {

	public:

		/// @brief Genera el simplex final a partir
		/// de los vol�menes @p first y @p second.
		/// @return Simplex finalizado.
		static Simplex GenerateFrom(
			const IGjkCollider& first,
			const IGjkCollider& second);

		/// @brief A�ade un nuevo v�rtice y actualiza
		/// el estado interno.
		/// @param point Nuevo v�rtice.
		void AddAndUpdate(const MinkowskiSupport& point);

		/// @return Direcci�n en la que hay que buscar
		/// el siguiente v�rtice del hull GJK.
		Vector3f GetSearchDirection() const;

		/// @return V�rtices del simplex.
		std::span<const MinkowskiSupport> GetVertices() const;

		/// @return N�mero de v�rtices del simplex.
		USize64 GetVerticesCount() const;

		/// @return True si el simplex contiene el origen
		/// de coordenadas (0, 0, 0).
		bool ContainsOrigin() const;

	private:

		/// @brief A�adde un nuevo punto al simplex.
		/// @param point Nuevo v�rtice.
		void AddPoint(const MinkowskiSupport& point);

		/// @param a Vector A.
		/// @param b Vector B.
		/// @return True si A.Dot(B) >= 0.0f.
		static bool IsSameDirection(const Vector3f& a, const Vector3f& b);


		/// @brief Actualizaci�n de estado interno en el caso
		/// de que s�lamente tenga un �nico punto.
		void UpdatePoint();

		/// @brief Actualizaci�n de estado interno en el caso
		/// de que tenga una l�nea.
		void UpdateLine();

		/// @brief Actualizaci�n de estado interno en el caso
		/// de que tenga un tri�ngulo.
		void UpdateTriangle();

		/// @brief Actualizaci�n de estado interno en el caso
		/// de que tenga una tetraedro.
		void UpdateTetrahedron();


		std::array<MinkowskiSupport, 4> m_vertices = {};
		USize64 m_verticesCount = 0;

		Vector3f m_direction = Vector3f(1.0f, 0.0f, 0.0f);
		bool m_containsOrigin = false;

	};

}
