#pragma once

#include <string>

#include "IToken.h"
#include "Transform.h"
#include "Scanner.h"
#include "Vector3TokenConverter.h"

namespace OSK::SceneSystem::Loader {

	/// <summary>
	/// Un token que representa un comando de spawn.
	/// </summary>
	class PlaceToken : public IToken {

	public:

		/// <summary>
		/// Token vacío.
		/// </summary>
		PlaceToken();

		/// <summary>
		/// Procesa la información del token.
		/// </summary>
		void ProcessData(Scanner* sc) override;

		/// <summary>
		/// Devuelve el nombre de la clase del GameObject.
		/// </summary>
		std::string GetClassName() const;

		/// <summary>
		/// Devuelve el nombre que tiene que tener la instancia.
		/// </summary>
		std::string GetInstanceName() const;

		/// <summary>
		/// Devuelve la posición en la que debe estar el objeto.
		/// </summary>
		Vector3f GetPosition() const;

		/// <summary>
		/// El eje sobre el que se va a girar el ángulo inicial.
		/// </summary>
		Vector3f GetAxis() const;

		/// <summary>
		/// El ángulo inicial de giro.
		/// </summary>
		float GetAngle() const;

		/// <summary>
		/// Tamaño de la entidad.
		/// </summary>
		Vector3f GetScale() const;

	private:

		/// <summary>
		/// El nombre de la clase del GameObject.
		/// </summary>
		std::string className;

		/// <summary>
		/// El nombre que tiene que tener la instancia.
		/// </summary>
		std::string instanceName;

		/// <summary>
		/// La posición en la que debe estar el objeto.
		/// </summary>
		Vector3f position;

		/// <summary>
		/// El eje sobre el que se va a girar el ángulo inicial.
		/// </summary>
		Vector3f axis;

		/// <summary>
		/// El ángulo inicial de giro.
		/// </summary>
		float angle;

		/// <summary>
		/// Tamaño de la entidad.
		/// </summary>
		Vector3f scale;

	};

}