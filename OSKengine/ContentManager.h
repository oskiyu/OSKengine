#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

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
#include "SoundEntity.h"

#include <list>

namespace OSK {

	class RenderAPI;

	//ContentManager es una clase que se encarga de cargar los recursos del juego.
	//Almacena los recursos que se hayan cargado, y los elimina al cerrarse (o al llamar a Unload()).
	//Pueden usarse varios, para cargar recursos por lotes que tienen el mismo periodo de vida.
	struct OSKAPI_CALL ContentManager {

		friend class RenderAPI;

		//Directorio en el que se guarda la textura por defecto.
		static const std::string DEFAULT_TEXTURE_PATH;

		//Crea un ContentManager vac�o.
		//	<renderer>: renderizador del juego.
		ContentManager(RenderAPI* renderer);

		//Elimina el ContentManager.
		//Llama a Unload().
		~ContentManager();

		//Carga una textura 2D (para un sprite).
		//	<path>: ruta de la textura (incluyendo la extensi�n de la imagen).
		Texture* LoadTexture(const std::string& path);

		//Carga una textura 2D (para un modelo 3D).
		//	<path>: ruta de la textura (incluyendo la extensi�n de la imagen).
		ModelTexture* LoadModelTexture(const std::string& path);

		//Carga un cubemap para un Skybox.
		//	<path>: ruta de la textura (incluyendo la extensi�n de la imagen).
		SkyboxTexture* LoadSkyboxTexture(const std::string& path);

		//Carga los v�rtices y los �ndices de un modelo 3D.
		//	<path>: ruta del modelo (incluyendo la extensi�n del modelo).
		//	<scala>: escala del modelo 3D.
		TempModelData GetModelTempData(const std::string& path, const float_t& scale = 1.0f) const;

		//Crea un modelo 3D a partir de sus v�rtices y sus �ndices.
		//	<v�rtices>: v�rtices.
		//	<�ndices>: �ndices (vertexIndex_t).
		ModelData* CreateModel(const std::vector<Vertex>& vertices, const std::vector<vertexIndex_t>& indices);

		//Carga un modelo 3D con sus buffers creados.
		//Solamente existe una instancia de ModelData* por cada <path>.
		//	<path>: ruta del modelo (incluyendo la extensi�n del modelo).
		//	<scala>: escala del modelo 3D.
		ModelData* LoadModelData(const std::string& path);

		//Carga un sprite.
		//	<sprite>: sprite a cargar.
		//	<path>: ruta de la textura (incluyendo la extensi�n).
		void LoadSprite(Sprite& sprite, const std::string& path);

		//Crea un sprite vac�o.
		void CreateSprite(Sprite& sprite);

		//Carga una fuente.
		//	<font>: fuente a cargar.
		//	<source>: ruta del archivo (incluyendo la extensi�n).
		//	<size>: tama�o al que se va a cargar la fuente, en p�xeles.
		Font* LoadFont(const std::string& source, uint32_t size);

		//Carga un skybox.
		//	<skybox>: skybox a cargar.
		//	<path>: ruta de la textura (incluyendo la extensi�n).
		void LoadSkybox(Skybox& skybox, const std::string& path);

		//Carga un modelo 3D.
		//Las texturas del modelo deben estar en la misma carpeta que el modelo.
		//	<model>: modelo a cargar.
		//	<path>: ruta del modelo (incluyendo la extensi�n del modelo).
		void LoadModel(Model& model, const std::string& path);

		//Carga un modelo 3D animado.
		//	<model>: modelo animado a cargar.
		//	<path>: ruta del modelo (incluyendo la extensi�n del modelo).
		AnimatedModel* LoadAnimatedModel(const std::string& path);

		//Carga un heightmap.
		//	<map>: heightmap a cargar.
		//	<path>: ruta del archivo (con extensi�n).
		//El heightmap NO es propiedad del ContentManager y no se eliminar� al llamar a Unload().
		void LoadHeightmap(Heightmap& map, const std::string& path);

		SoundEntity* LoadSoundEntity(const std::string& path);

		//Elimina todos los recursos almacenados.
		void Unload();

		//Textura por defecto.
		static ModelTexture* DefaultTexture;

	private:

		//Almacena las texturas 2D.
		std::list<Texture*> Textures = {};
		//Almacena las texturas de modelos.
		std::list<ModelTexture*> ModelTextures = {};
		//Almacena las texturas de Skybox.
		std::list<SkyboxTexture*> SkyboxTextures = {};
		//Almacena los v�rtices y los �ndices de los modelos 3D.
		std::list<ModelData*> ModelDatas = {};
		//Almacena los v�rtices y los �ndices de los modelos 3D animados.
		std::list<AnimatedModel*> AnimatedModels = {};
		//Almacena referencias a los sprites.
		std::list<Sprite*> Sprites = {};
		//Almacena referencias a las fuentes.
		std::list<Font*> Fonts = {};
		//Almacena referencias a los sonidos.
		std::list<SoundEntity*> Sounds = {};
		
		//HASH-MAPS.
		std::unordered_map<std::string, Texture*> TextureFromPath = {};
		std::unordered_map<std::string, ModelTexture*> ModelTextureFromPath = {};
		std::unordered_map<std::string, SkyboxTexture*> SkyboxTextureFromPath = {};
		std::unordered_map<std::string, ModelData*> ModelDataFromPath = {};
		std::unordered_map<std::string, AnimatedModel*> AnimatedModelFromPath = {};
		std::unordered_map<std::string, Font*> FontsFromPath = {};
		std::unordered_map<std::string, SoundEntity*> SoundsFromPath = {};

		//El m�ximo n�mero de mipmaps que se puede generar para una textura.
		//	<width>: ancho de la textura.
		//	<height>: alto de la textura.
		inline const uint32_t getMaxMipLevels(const uint32_t& width, const uint32_t& height) const {
			return std::floor(std::log2(std::max(width, height))) + 1;
		}

		bool hasBeenCleared = false;
		RenderAPI* renderer;

		static Assimp::Importer GlobalImporter;
		const static int AssimpFlags = aiProcess_Triangulate | aiProcess_GenNormals;

	};

}