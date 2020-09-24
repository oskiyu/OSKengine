#pragma once

#include "Texture.h"
#include "Model.h"
#include "AnimatedModel.h"
#include "Skybox.h"
#include "Sprite.h"
#include "Font.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Heightmap.h"


namespace OSK {

	class VulkanRenderer;

	//ContentManager es una clase que se encarga de cargar los recursos del juego.
	//Almacena los recursos que se hayan cargado, y los elimina al cerrarse (o al llamar a Unload()).
	//Pueden usarse varios, para cargar recursos por lotes que tienen el mismo periodo de vida.
	struct ContentManager {

		friend class VulkanRenderer;

		static const std::string DEFAULT_TEXTURE_PATH;

		//Crea un ContentManager vacío.
		//	<renderer>: renderizador del juego.
		ContentManager(VulkanRenderer* renderer);

		//Elimina el ContentManager.
		//Llama a Unload().
		~ContentManager();

		//Carga una textura 2D (para un sprite).
		//	<path>: ruta de la textura (incluyendo la extensión de la imagen).
		Texture* LoadTexture(const std::string& path);

		//Carga una textura 2D (para un modelo 3D).
		//	<path>: ruta de la textura (incluyendo la extensión de la imagen).
		ModelTexture* LoadModelTexture(const std::string& path);

		//Carga un cubemap para un Skybox.
		//	<path>: ruta de la textura (incluyendo la extensión de la imagen).
		SkyboxTexture* LoadSkyboxTexture(const std::string& path);

		//Carga los vértices y los índices de un modelo 3D.
		//	<path>: ruta del modelo (incluyendo la extensión del modelo).
		//	<scala>: escala del modelo 3D.
		TempModelData GetModelTempData(const std::string& path, const float_t& scale = 1.0f) const;

		//Crea un modelo 3D a partir de sus vértices y sus índices.
		//	<vértices>: vértices.
		//	<índices>: índices (vertexIndex_t).
		ModelData* CreateModel(const std::vector<Vertex>& vertices, const std::vector<vertexIndex_t>& indices);

		//Carga un modelo 3D con sus buffers creados.
		//Solamente existe una instancia de ModelData* por cada <path>.
		//	<path>: ruta del modelo (incluyendo la extensión del modelo).
		//	<scala>: escala del modelo 3D.
		ModelData* LoadModelData(const std::string& path);

		//Carga un sprite.
		//	<sprite>: sprite a cargar.
		//	<path>: ruta de la textura (incluyendo la extensión).
		void LoadSprite(Sprite& sprite, const std::string& path);

		//Carga una fuente.
		//	<font>: fuente a cargar.
		//	<source>: ruta del archivo (incluyendo la extensión).
		//	<size>: tamaño al que se va a cargar la fuente, en píxeles.
		void LoadFont(Font& fuente, const std::string& source, uint32_t size);

		//Carga un skybox.
		//	<skybox>: skybox a cargar.
		//	<path>: ruta de la textura (incluyendo la extensión).
		void LoadSkybox(Skybox& skybox, const std::string& path);

		//Carga un modelo 3D.
		//Las texturas del modelo deben estar en la misma carpeta que el modelo.
		//	<model>: modelo a cargar.
		//	<path>: ruta del modelo (incluyendo la extensión del modelo).
		void LoadModel(Model& model, const std::string& path);

		//Carga un modelo 3D animado.
		//	<model>: modelo animado a cargar.
		//	<path>: ruta del modelo (incluyendo la extensión del modelo).
		void LoadAnimatedModel(AnimatedModel& model, const std::string& path);

		//Carga un heightmap.
		//	<map>: heightmap a cargar.
		//	<path>: ruta del archivo (con extensión).
		void LoadHeightmap(Heightmap& map, const std::string& path);

		//Elimina todos los recursos almacenados.
		void Unload();

		static ModelTexture* DefaultTexture;

	private:

		//Almacena las texturas 2D.
		std::vector<Texture*> Textures = {};
		//Almacena las texturas de modelos.
		std::vector<ModelTexture*> ModelTextures = {};
		//Almacena las texturas de Skybox.
		std::vector<SkyboxTexture*> SkyboxTextures = {};
		//Almacena los vértices y los índices de los modelos 3D.
		std::vector<ModelData*> ModelDatas = {};
		//Almacena referencias a los sprites.
		std::vector<Sprite*> Sprites = {};
		
		//HASH-MAPS.
		std::unordered_map<std::string, Texture*> TextureFromPath = {};
		std::unordered_map<std::string, ModelTexture*> ModelTextureFromPath = {};
		std::unordered_map<std::string, SkyboxTexture*> SkyboxTextureFromPath = {};
		std::unordered_map<std::string, ModelData*> ModelDataFromPath = {};

		//El máximo número de mipmaps que se puede generar para una textura.
		//	<width>: ancho de la textura.
		//	<height>: alto de la textura.
		inline const uint32_t getMaxMipLevels(const uint32_t& width, const uint32_t& height) const {
			return std::floor(std::log2(std::max(width, height))) + 1;
		}

		bool hasBeenCleared = false;
		VulkanRenderer* renderer;

		static Assimp::Importer GlobalImporter;
		const static int AssimpFlags = aiProcess_Triangulate | aiProcess_GenNormals;

	};

}
