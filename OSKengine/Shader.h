#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "DirectionalLight.h"
#include "Material.h"

#include <glm.hpp>
#include <glad/glad.h>

#include <string>

namespace OSK {

	//Shader para el renderizado.
	class OSKAPI_CALL Shader {

	public:

		//Crea un shader vacío.
		Shader();


		//Usar este shader.
		void Use();


		//Envía un bool al shader.	
		void SetBool(const std::string& name, const bool& val) const;


		//Envía un int al shader.	
		void SetInt32(const std::string& name, const int32_t& val) const;


		//Envía un float al shader.	
		void SetFloat(const std::string& name, const float_t& val) const;

		
		//Envía una matriz al shader.	
		void SetMat4(const std::string& name, const glm::mat4& mat) const;

		
		//Establece la matriz proyección.	
		void SetProjection(const glm::mat4&) const;


		//Establece la matriz vista.	
		void SetView(const glm::mat4&) const;

		
		//Establece la matriz de modelo.
		void SetModel(const glm::mat4&) const;

		
		//Envía un vector 2 al shader.	
		void SetVec2(const std::string& name, const Vector2& value) const;


		//Envía un vector 2 al shader.
		void SetVec2(const std::string& name, const float_t& x, const float_t& y) const;


		//Envía un vector 3 al shader.
		void SetVec3(const std::string& name, const Vector3& value) const;


		//Envía un vector 3 al shader.
		void SetVec3(const std::string& name, const float_t& x, const float_t& y, const float_t& z) const;


		//Envía un vector 4 al shader.
		void SetVec4(const std::string& name, const Vector4& value) const;


		//Envía un vector 4 al shader.
		void SetVec4(const std::string& name, const float_t& x, const float_t& y, const float_t& z, const float_t& w) const;


		//Envía un color al shader.
		void SetColor(const std::string& name, const Color& color) const;
		

		//Establece una luz direccional.
		void SetDirectionalLight(const DirectionalLight& light) const;


		//Establece un material para el renderizado.
		void SetMaterial(const Material& material) const;


		//OpenGL.
		OSK_INFO_DO_NOT_TOUCH
			uint32_t ProgramID = 0;

	private:

	};

}