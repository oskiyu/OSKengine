#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "SAT_Projection.hpp"
#include "SAT_CollisionInfo.h"
#include "Transform.h"

#include <vector>

namespace OSK::Collision {

	/// <summary>
	/// Lista de puntos que representan una cara.
	/// </summary>
	typedef std::vector<uint32_t> SAT_Face;

	/// <summary>
	/// Collider que usa el Teorema de Separaci�n de Ejes.
	/// Puede tener n caras, cada una con m puntos.
	/// </summary>
	class OSKAPI_CALL SAT_Collider {

	public:

		/// <summary>
		/// Optimiza las caras: elimina caras duplicadas, dejando s�lamente las que tienen ejes �nicos.
		/// </summary>
		void OptimizeFaces();

		/// <summary>
		/// A�ade una nueva cara al collider.
		/// </summary>
		/// <param name="points">Array de puntos que componen la cara.</param>
		/// <param name="size">N�mero de puntos que componen la cara.</param>
		void AddFace(const Vector3f points[], uint32_t size);

		/// <summary>
		/// Comprueba si este collider choca con otro.
		/// M�s barato de ejecutar que GetCollisionInfo.
		/// </summary>
		/// <param name="other">Otro collider.</param>
		/// <returns>True si est�n colisionando.</returns>
		bool Intersects(const SAT_Collider& other) const;

		/// <summary>
		/// Obtiene un SAT_CollisionInfo con informaci�n detallada de la intersecci�n de este collider con otro.
		/// </summary>
		/// <param name="other">Otro collider.</param>
		/// <returns>Informaci�n de la colisi�n.</returns>
		SAT_CollisionInfo GetCollisionInfo(const SAT_Collider& other) const;

		/// <summary>
		/// Actualiza la posici�n de los puntos en el mundo.
		/// </summary>
		void TransformPoints();

		/// <summary>
		/// Transform del colisionador.
		/// </summary>
		Transform BoxTransform{};

		/// <summary>
		/// Crea un SAT_Collider c�bico.
		/// </summary>
		/// <returns>Colisionador c�bico.</returns>
		static SAT_Collider CreateOBB();

		/// <summary>
		/// Obtiene los puntos transformados en el mundo.
		/// </summary>
		/// <returns>Puntos en el mundo.</returns>
		std::vector<Vector3f> GetPoints() const;

		//Obtiene los ejes del collider.
		//Transformados teniendo en cuenta su posici�n y rotaci�n.

		/// <summary>
		/// Obtiene los ejes del collider.
		/// Transformados teniendo en cuenta su posici�n y rotaci�n.
		/// </summary>
		/// <returns>Ejes de las caras del colisionador.</returns>
		std::vector<Vector3f> GetAxes() const;

		/// <summary>
		/// True si el collider no va a moverse, rotar o cambiar de tama�o.
		/// Para la optimizaci�n de transformaci�n de puntos.
		/// </summary>
		bool IsStatic = false;

		/// <summary>
		/// Proyecta este colisionador sobre un eje.
		/// </summary>
		/// <param name="axis">Eje.</param>
		/// <returns>Proyecci�n.</returns>
		SAT_Projection ProjectToAxis(const Vector3f& axis) const;

	private:
		
		/// <summary>
		/// Obtiene el eje de una cara.
		/// </summary>
		/// <param name="face">Cara.</param>
		/// <returns>Eje.</returns>
		Vector3f GetAxisFromFace(const SAT_Face& face) const;

		/// <summary>
		/// Puntos del colisionador, en espacio local.
		/// </summary>
		std::vector<Vector3f> Points = {};

		/// <summary>
		/// Puntos del colisionador, en espacio del mundo.
		/// </summary>
		std::vector<Vector3f> TransformedPoints = {};

		/// <summary>
		/// Caras del colisionador.
		/// </summary>
		std::vector<SAT_Face> Faces;

	};

}