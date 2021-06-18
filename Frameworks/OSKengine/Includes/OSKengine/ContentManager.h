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
#include "TempModelData.h"

#include <list>

namespace OSK {

	class RenderAPI;

	/// <summary>
	/// ContentManager es una clase que se encarga de cargar los recursos del juego.
	/// Almacena los recursos que se hayan cargado, y los elimina al cerrarse (o al llamar a Unload()).
	/// Pueden usarse varios, para cargar recursos por lotes que tienen el mismo periodo de vida.
	/// </summary>
	class OSKAPI_CALL ContentManager {

		friend class RenderAPI;

	public:

		/// <summary>
		/// Directorio en el que se guarda la textura por defecto.
		/// </summary>
		static const std::string DEFAULT_TEXTURE_PATH;

		/// <summary>
		/// Crea un ContentManager vacío.
		/// </summary>
		/// <param name="renderer">Renderizador del juego.</param>
		ContentManager(RenderAPI* renderer);

		/// <summary>
		/// Elimina el ContentManager.
		/// Llama a Unload().
		/// <summary/>
		~ContentManager();
		
		/// <summary>
		/// Carga y almacena una textura. 
		/// Si la textura ya ha sido cargada por este ContentManager, devuelve la textura sin volver a cargarla.
		/// </summary>
		/// <param name="path">Localización de la textura (con extensión (por ejemplo, .png)).</param>
		/// <returns>Puntero a la textura.</returns>
		Texture* LoadTexture(const std::string& path, TextureFilterType filter = TextureFilterType::LINEAR);

		/// <summary>
		/// Carga y almacena una textura de skybox. 
		/// Si la textura ya ha sido cargada por este ContentManager, devuelve la textura sin volver a cargarla.
		/// La carpeta debe tener las imagenes tal que: <para/>
		/// right.jpp		 <para/>
		///	left.jpg		 <para/>
		///	top.jpg			 <para/>
		///	bottom.jpg		 <para/>
		///	front.jpg		 <para/>
		///	back.jpg		 <para/>
		/// </summary>
		/// <param name="path">Localización de las texturas (la carpeta).</param>
		/// <returns>Puntero a la textura.</returns>
		Texture* LoadSkyboxTexture(const std::string& folderPath);

		/// <summary>
		/// Carga los vértices y los índices de un modelo 3D.
		/// </summary>
		/// <param name="path">Ruta del modelo (incluyendo la extensión del modelo).</param>
		/// <param name="scale">Escala del modelo 3D.</param>
		/// <returns>Data del modelo.</returns>
		TempModelData GetModelTempData(const std::string& path, float scale = 1.0f) const;

		/// <summary>
		/// Crea un modelo 3D a partir de sus vértices y sus índices.
		/// </summary>
		/// <param name="vertices">Vértices.</param>
		/// <param name="indices">Índices (vertexIndex_t).</param>
		/// <returns>Data del modelo.</returns>
		ModelData* CreateModel(const std::vector<Vertex>& vertices, const std::vector<vertexIndex_t>& indices);

		/// <summary>
		/// Carga un modelo 3D.
		/// Si el modelo ya ha sido cargada por este ContentManager, devuelve el modelo sin volver a cargarlo.
		/// </summary>
		/// <param name="path">Ruta del modelo (incluyendo la extensión del modelo).</param>
		/// <returns>Modelo.</returns>
		ModelData* LoadModelData(const std::string& path);

		/// <summary>
		/// Carga un sprite.
		/// </summary>
		/// <param name="sprite">Sprite a cargar.</param>
		/// <param name="path">Ruta de la textura (incluyendo la extensión).</param>
		void LoadSprite(Sprite* sprite, const std::string& path);

		/// <summary>
		/// Crea un sprite vacío.
		/// </summary>
		/// <param name="sprite">Sprite</param>
		void CreateSprite(Sprite* sprite);

		/// <summary>
		/// Carga una fuente.
		/// Si la fuente ya ha sido cargada por este ContentManager, devuelve la fuente sin volver a cargarla.
		/// </summary>
		/// <param name="source">Ruta del archivo (incluyendo la extensión).</param>
		/// <param name="size">Tamaño al que se va a cargar la fuente, en píxeles.</param>
		/// <returns>Fuente.</returns>
		Font* LoadFont(const std::string& source, uint32_t size);

		/// <summary>
		/// Carga un skybox.
		/// </summary>
		/// <param name="skybox">Skybox a cargar.</param>
		/// <param name="path">Localización de las texturas (la carpeta).</param>
		void LoadSkybox(Skybox* skybox, const std::string& path);

		/// <summary>
		/// Carga un modelo 3D.
		/// Si el modelo ya ha sido cargada por este ContentManager, devuelve el modelo sin volver a cargarlo.
		/// </summary>
		/// <param name="path">Ruta del modelo (incluyendo la extensión del modelo).</param>
		/// <returns>Modelo.</returns>
		void LoadModel(Model* model, const std::string& path);

		/// <summary>
		/// Carga un modelo 3D animado.
		/// Si el modelo ya ha sido cargada por este ContentManager, devuelve el modelo sin volver a cargarlo.
		/// </summary>
		/// <param name="path">Ruta del modelo (incluyendo la extensión del modelo).</param>
		/// <returns>Modelo.</returns>
		void LoadAnimatedModel(AnimatedModel* model, const std::string& path);

		/// <summary>
		/// Carga un heightmap.
		/// El heightmap NO es propiedad del ContentManager y no se eliminará al llamar a Unload().
		/// </summary>
		/// <param name="map">Heightmap a cargar</param>
		/// <param name="path">Ruta del archivo (con extensión).</param>
		void LoadHeightmap(Heightmap* map, const std::string& path);

		/// <summary>
		/// Carga un sonido.
		/// Si el sonido ya ha sido cargada por este ContentManager, devuelve el sonido sin volver a cargarlo.
		/// </summary>
		/// <param name="path">Ruta del sonido (con extensión).</param>
		/// <returns>Sonido.</returns>
		SoundEmitterComponent* LoadSoundEntity(const std::string& path);

		/// <summary>
		/// Elimina todos los recursos almacenados.
		/// </summary>
		void Unload();

		/// <summary>
		/// Textura por defecto de OSKengine.
		/// </summary>
		static Texture* DefaultTexture;

	private:

		/// <summary>
		/// Almacena las texturas.
		/// </summary>
		std::list<Texture*> textures = {};

		/// <summary>
		/// Almacena los vértices y los índices de los modelos 3D.
		/// </summary>
		std::list<ModelData*> modelDatas = {};

		/// <summary>
		/// Almacena referencias a los sprites.
		/// </summary>
		std::list<Sprite*> sprites = {};

		/// <summary>
		/// Alacena las fuentes.
		/// </summary>
		std::list<Font*> fonts = {};
		
		/// <summary>
		/// Almacena los sonidos.
		/// </summary>
		std::list<SoundEmitterComponent*> sounds = {};
		
		//HASH-MAPS.

		/// <summary>
		/// Hashmap de las texturas.
		/// </summary>
		std::unordered_map<std::string, Texture*> textureFromPath = {};

		/// <summary>
		/// Hashmap de los modelos.
		/// </summary>
		std::unordered_map<std::string, ModelData*> modelDataFromPath = {};

		/// <summary>
		/// Hashmap de los modelos animados.
		/// </summary>
		std::unordered_map<std::string, AnimatedModel*> animatedModelFromPath = {};

		/// <summary>
		/// Hashmap de las fuentes.
		/// </summary>
		std::unordered_map<std::string, Font*> fontsFromPath = {};

		/// <summary>
		/// Hashmap de los sonidos.
		/// </summary>
		std::unordered_map<std::string, SoundEmitterComponent*> soundsFromPath = {};

		/// <summary>
		/// El máximo número de mipmaps que se puede generar para una textura.
		/// </summary>
		/// <param name="width">Ancho de la textura.</param>
		/// <param name="height">Alto de la textura.</param>
		/// <returns>Número de mipmaps.</returns>
		inline const uint32_t GetMaxMipLevels(uint32_t width, uint32_t height) const {
			return (uint32_t)std::floor(std::log2(std::max(width, height))) + 1;
		}

		/// <summary>
		/// Devuelve los nodos en formato OSK a partir del nodo de assimp.
		/// </summary>
		/// <param name="node">Nodo assimp.</param>
		/// <returns>Nodo formato OSK.</returns>
		Animation::SNode GetNodes(const aiNode* node);

		/// <summary>
		/// True si el content manager se ha liberado.
		/// </summary>
		bool hasBeenCleared = false;

		/// <summary>
		/// Renderizador.
		/// </summary>
		RenderAPI* renderer = nullptr;

		/// <summary>
		/// Importador de assimp, para modelos 3D.
		/// </summary>
		static Assimp::Importer globalImporter;

		/// <summary>
		/// Flags de assimp.
		/// </summary>
		const static int ASSIMP_FLAGS = aiProcess_Triangulate | aiProcess_GenNormals;

	};

}
