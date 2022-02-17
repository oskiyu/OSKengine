#include <json.hpp>

#include <iostream>

#include <fstream>
#include <string>

	/// <summary>
	/// Clase que actúa de interfaz para la lectura y escritura de atchivos.
	/// </summary>
	class FileIO {

	public:

		/// <summary>
		/// Escribe un archivo de texto.
		/// </summary>
		/// <param name="path">Ruta del archivo (con extensión).</param>
		/// <param name="text">Texto que se va a escribir.</param>
		static void WriteFile(const std::string& path, const std::string& text);

		/// <summary>
		/// Lee un archivo de texto.
		/// </summary>
		/// <param name="path">Ruta del archivo (con extensión).</param>
		/// <returns>String con el contenido del archivo.</returns>
		static std::string ReadFromFile(const std::string& path);

		/// <summary>
		/// Comprueba si un archivo existe.
		/// </summary>
		/// <param name="path">Ruta del archivo (con extensión).</param>
		/// <returns>True si existe.</returns>
		static bool FileExists(const std::string& path);

	};

	void FileIO::WriteFile(const std::string& path, const std::string& text) {
		std::ofstream stream(path);
		stream << text << std::endl;

		stream.close();
	}


	std::string FileIO::ReadFromFile(const std::string& path) {
		std::ifstream stream(path);
		std::string line;
		std::string ret = "";

		while (std::getline(stream, line)) {
			ret.append(line);
			ret.append("\n");
		}

		stream.close();

		return ret;
	}

	bool FileIO::FileExists(const std::string& path) {
		std::ifstream stream(path);

		bool output = stream.good();

		if (stream.is_open())
			stream.close();

		return output;
};


struct Binding {
	std::string name;
	std::string type;
};

struct Slot {
	std::string name;
	std::vector<Binding> bindings;
};

struct Material {
	std::string name;
	std::string shaderFile;

	std::vector<Slot> slots;
};


int main()  {
	std::string text = FileIO::ReadFromFile("material.json");
	nlohmann::json materialInfo = nlohmann::json::parse(text);

	int fileVersion = materialInfo["spec_ver"];

	Material material{};
	material.name = materialInfo["name"];
	material.shaderFile = materialInfo["shader_file"];

	for (auto& slotInfo : materialInfo["layout"]["slots"]) {
		Slot slot{};
		slot.name = slotInfo["name"];

		for (auto& bindingInfo : slotInfo["bindings"]) {
			Binding binding{};
			binding.name = bindingInfo["name"];
			binding.type = bindingInfo["type"];
			
			slot.bindings.push_back(binding);
		}

		material.slots.push_back(slot);
	}

	//  SHADER
	nlohmann::json shaderInfo = nlohmann::json::parse(FileIO::ReadFromFile("shader.json"));

	fileVersion = shaderInfo["spec_ver"];
	
	for (auto& setInfo : shaderInfo["sets"]) {
		std::string name = setInfo["name"];

		for (auto& bindingInfo : setInfo["bindings"]) {
			std::string name = bindingInfo["name"];

			int glslSet = bindingInfo["glsl"]["set"];
			int glslId = bindingInfo["glsl"]["index"];

			int hlslId = bindingInfo["hlsl"]["index"];
		}
	}

	return 0;
}
