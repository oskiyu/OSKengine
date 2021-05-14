#pragma once

#include "OSKmacros.h"

#include "Vector3.hpp"
#include "Scanner.h"
#include "Scanner.h"

namespace OSK::SceneSystem::Loader {

	class Vector3TokenConverter {

	public:

		/// <summary>
		/// Procesa el tecto que contiene un vector 3D.
		/// </summary>
		/// <param name="sdata">VEctor 3D entre par�ntesis.</param>
		void Process(const std::string& sdata);

		/// <summary>
		/// Devuelve el vector.
		/// </summary>
		Vector3f GetVector3() const;

	private:

		/// <summary>
		/// Obtiene el pr�ximo n�mero.
		/// Debe de haberse consumido cualquier s�mbolo antes.
		/// </summary>
		float GetNextNumber();
		
		/// <summary>
		/// Consume los espacios.
		/// </summary>
		void ConsumeSpace();
		
		/// <summary>
		/// Devuelve el pr�ximo char.
		/// </summary>
		char NextChar() const;

		/// <summary>
		/// Cursor que apunta al char que se est� procesando en un momento dado.
		/// </summary>
		int charPos = 0;

		/// <summary>
		/// Data.
		/// </summary>
		std::string data;

		/// <summary>
		/// Coordenada X.
		/// </summary>
		float x = 0;

		/// <summary>
		/// Coordenada Y.
		/// </summary>
		float y = 0;

		/// <summary>
		/// Coordenada Z.
		/// </summary>
		float z = 0;

	};

}