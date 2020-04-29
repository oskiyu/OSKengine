#include "Shader.h"

namespace OSK {

	Shader::Shader() {

	}


	void Shader::Use() {
		glUseProgram(ProgramID);
	}


	void Shader::SetBool(const std::string& name, const bool& val) const {
		glUniform1i(glGetUniformLocation(ProgramID, name.c_str()), (int)val);
	}


	void Shader::SetInt32(const std::string& name, const int32_t& val) const {
		glUniform1i(glGetUniformLocation(ProgramID, name.c_str()), val);
	}


	void Shader::SetFloat(const std::string& name, const float_t& val) const {
		glUniform1f(glGetUniformLocation(ProgramID, name.c_str()), val);
	}


	void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const {
		glUniformMatrix4fv(glGetUniformLocation(ProgramID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}


	void Shader::SetProjection(const glm::mat4& mat) const {
		glUniformMatrix4fv(glGetUniformLocation(ProgramID, "projection"), 1, GL_FALSE, &mat[0][0]);
	}


	void Shader::SetView(const glm::mat4& mat) const {
		glUniformMatrix4fv(glGetUniformLocation(ProgramID, "view"), 1, GL_FALSE, &mat[0][0]);
	}


	void Shader::SetModel(const glm::mat4& mat) const {
		glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, &mat[0][0]);
	}


	void Shader::SetVec2(const std::string& name, const Vector2& value) const {
		SetVec2(name, value.X, value.Y);
	}


	void Shader::SetVec2(const std::string& name, const float_t& x, const float_t& y) const {
		glUniform2f(glGetUniformLocation(ProgramID, name.c_str()), x, y);
	}


	void Shader::SetVec3(const std::string& name, const Vector3& value) const {
		SetVec3(name, value.X, value.Y, value.Z);
	}


	void Shader::SetVec3(const std::string& name, const float_t& x, const float_t& y, const float_t& z) const {
		glUniform3f(glGetUniformLocation(ProgramID, name.c_str()), x, y, z);
	}


	void Shader::SetVec4(const std::string& name, const Vector4& value) const {
		SetVec4(name, value.X, value.Y, value.Z, value.W);
	}


	void Shader::SetVec4(const std::string& name, const float_t& x, const float_t& y, const float_t& z, const float_t& w) const {
		glUniform4f(glGetUniformLocation(ProgramID, name.c_str()), x, y, z, w);
	}


	void Shader::SetColor(const std::string& name, const Color& color) const {
		glUniform4f(glGetUniformLocation(ProgramID, name.c_str()), color.Red, color.Green, color.Blue, color.Alpha);
	}


	void Shader::SetDirectionalLight(const DirectionalLight& light) const {
		SetVec3("dirLight.Direction", light.Direction);
		SetColor("dirLight.Ambient", light.Color);
		SetFloat("dirLight.Intensity", light.Intensity);
	}


	void Shader::SetMaterial(const Material& material) const {
		SetInt32("material.Diffuse", material.Diffuse);
		SetInt32("material.Specular", material.Specular);
		SetColor("material.Ambient", material.Ambient);
		SetFloat("material.Shininess", material.Shininess);
	}

}