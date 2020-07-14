#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "OldFont.h"
#include "Model.h"
#include "ModelFormat.h"
#include "Shader.h"
#include "Skybox.h"
#include "SoundEntity.h"
#include "OldTexture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <array>

namespace OSK {

	//Clase que carga el contenido del juego.
	class OSKAPI_CALL ContentAPI {

	public:

		//Crea una instancia ContentAPI.
		ContentAPI(const std::string& path = "");


		//Destruye este ContentAPI.
		~ContentAPI();


		//Carga un modelo desde el directorio en el que se encuentra.
		void LoadModel(Model& model, const std::string& path);


		//Carga una textura desde el directorio en la que se encuentra.
		void LoadTexture(OldTexture& texture, const std::string& path);


		//Carga un Skybox desde 6 texturas individuales.
		void LoadSkybox(Skybox& skybox, const std::array<std::string, 6> faces);


		//Carga un shader desde un archivo.
		void LoadShaderFromFile(Shader& shader, const std::string& vertexPath, const std::string& shaderPath);


		//Carga un shader dado su código fuente.
		void LoadShaderFromSourceCode(Shader& shader, const std::string& vertexCode, const std::string& fragmentCode);


		//Carga audio para ser reproducido.
		//Formato = .wav (!)
		void LoadSound(SoundEntity& audio, const std::string& source);


		//Carga una fuente.
		void LoadFont(OldFont& fuente, const std::string& source, const int32_t size = 48);


		//Directorio base.
		std::string Directory = "";


		OSK_INFO_NOT_DEVELOPED
			std::string ModelDirectory = "/models";


		OSK_INFO_NOT_DEVELOPED
			std::string TextureDirectory = "/textures";
			   

		//Si es true, cada contenido que cargue será exportado como un .xd.
		bool ExportAsXd = true;

	private:

		void processNode(aiNode*, const aiScene*, Model&);

		void processMesh(aiMesh*, const aiScene*, Model&);

		void processMeshes(aiNode*, const aiScene*, Model&);

		void addNode(aiNode*, std::vector<aiNode*>&);

		/*aiNode* findAiNode(const std::string& name);

		aiNodeAnim* findAiNodeAnim(const std::string& name);

		OSK::Bone* findBone(const std::string& name);*/

	};

}