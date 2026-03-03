#pragma once

#include "ApiCall.h"

#include "UniquePtr.hpp"
#include "GpuBuffer.h"

#include "HashMap.hpp"
#include "VertexAttributes.h"

#include "GpuMesh3D.h"

#include <optional>
#include "Animator.h"

#include "CpuModel3D.h"

#include "AssetRef.h"
#include "Texture.h"

#include "Uuid.h"


namespace OSK::GRAPHICS {

	/// @brief Identificador único para un modelo de GPU.
	using GpuModelUuid = BaseUuid<class GpuModelUuidTag>;

	/// @brief Modelo para renderizado 3D.
	class OSKAPI_CALL GpuModel3D {

	public:

		/// @brief Indica los meshes pertenecientes al modelo
		/// en un nivel de detalle en concreto.
		struct Lod {

			/// @brief Nivel LOD más alto posible.
			constexpr static UIndex64 HighestLevel = 0;

			/// @brief Índice del primer mesh del LOD.
			UIndex64 firstMeshId = 0;

			/// @brief Número de meshes del LOD.
			USize64  meshesCount = 0;

		};


		/// @brief Información del material que puede tener un modelo.
		struct Material {
			Color baseColor = Color::White;
			Color emissiveColor = Color::Black * 0.0f;
			float roughnessFactor = 0.5f;
			float metallicFactor = 0.5f;

			std::optional<UIndex64> colorTextureIndex;
			std::optional<UIndex64> normalTextureIndex;
			std::optional<UIndex64> roughnessTextureIndex;
			std::optional<UIndex64> metallicTextureIndex;
		};


		/// @brief Almacena referencias a las texturas o imágenes que
		/// usará un material de un modelo 3D.
		class TextureTable {

		public:

			/// @param imageIndex Índice de la imagen.
			/// @return Image view de la imagen indicada.
			/// @pre @p imageIndex debe corresponderse con una imagen existente.
			const IGpuImageView* GetImageView(UIndex64 imageIndex) const;

			GpuImage* GetImage(UIndex64 imageIndex);

			/// @brief Establece la textura en el índice dado.
			/// @param index Índice de la imagen.
			/// @param texture Textura a enlazar.
			/// @note No actualiza el índice interno usado por
			/// AddTexture y AddGpuImageView.
			void SetTexture(
				UIndex64 index,
				ASSETS::AssetRef<ASSETS::Texture> texture);

			/// @brief Establece la imagen en el índice dado.
			/// @param index Índice de la imagen.
			/// @param view Imagen a enlazar.
			/// @note No actualiza el índice interno usado por
			/// AddTexture y AddGpuImageView.
			void SetGpuImageView(
				UIndex64 index,
				const IGpuImageView* view);


			/// @brief Añade una nueva textura a la tabla.
			/// Comienza en el índice 0, y cada llamada aumenta en 
			/// 1 el siguiente índice.
			/// @param texture Textura a enlazar.
			/// @note Puede sobreescribir imágenes enlazadas con
			/// SetTexture o SetGpuImageView.
			void AddTexture(ASSETS::AssetRef<ASSETS::Texture> texture);

			/// @brief Añade una nueva textura a la tabla.
			/// Comienza en el índice 0, y cada llamada aumenta en 
			/// 1 el siguiente índice.
			/// @param texture Textura a enlazar.
			/// @note Puede sobreescribir imágenes enlazadas con
			/// SetTexture o SetGpuImageView.
			void AddGpuImageView(const IGpuImageView* view);

		private:

			UIndex64 m_nextIndex = 0;

			std::unordered_map<UIndex64, ASSETS::AssetRef<ASSETS::Texture>> m_textures;
			std::unordered_map<UIndex64, const IGpuImageView*> m_imageViews;

		};

	public:

		/// @brief Establece el identificador único del modelo.
		/// @param uuid Indentificador único.
		void SetUuid(GpuModelUuid uuid);

		/// @return Identificador único de este modelo.
		GpuModelUuid GetUuid() const;


		/// @brief Establece el CpuModel3D a partir del cual se ha generado
		/// el modelo 3D.
		/// @param model modelo original.
		void SetCpuModel3D(const CpuModel3D& model);


		/// @return Modelo almacenado en CPU.
		/// Si no se ha establecido mediante SetCpuModel3D,
		/// devuelve un modelo vacío.
		CpuModel3D& GetCpuModel();

		/// @return Modelo almacenado en CPU.
		/// Si no se ha establecido mediante SetCpuModel3D,
		/// devuelve un modelo vacío.
		const CpuModel3D& GetCpuModel() const;


		/// @brief Establece el buffer con los vértices del modelo.
		/// @param vertexBuffer Buffer con los vértices del modelo.
		void SetVertexBuffer(UniquePtr<GpuBuffer>&& vertexBuffer);

		/// @brief Establece el buffer con los índices del modelo.
		/// @param vertexBuffer Buffer con los índices del modelo.
		void SetIndexBuffer(UniquePtr<GpuBuffer>&& indexBuffer);

		/// @brief Establece el número de índices total del modelo.
		/// @param indexCount Número de índices.
		void SetIndexCount(USize32 indexCount);


		/// @brief Añade una malla al modelo.
		/// @param mesh Malla.
		void AddMesh(GpuMesh3D&& mesh);

		/// @return Mallas del modelo.
		DynamicArray<GpuMesh3D>& GetMeshes();
		/// @return Mallas del modelo.
		const DynamicArray<GpuMesh3D>& GetMeshes() const;


		void SetMaterials(const DynamicArray<Material>& materials);

		/// @param materialIndex Índice del material.
		/// @return Material con el índice dado.
		/// 
		/// @pre Debe haberse establecido un CpuModel3D mediante SetCpuModel3D.
		/// @pre Debe existir un material con el índice indicado.
		Material& GetMaterial(UIndex64 materialIndex);

		/// @param materialIndex Índice del material.
		/// @return Material con el índice dado.
		/// 
		/// @pre Debe haberse establecido un CpuModel3D mediante SetCpuModel3D.
		/// @pre Debe existir un material con el índice indicado.
		const Material& GetMaterial(UIndex64 materialIndex) const;


		/// @return Buffer con los vértices en GPU.
		/// @pre Debe haber sido establecido con SetVertexBuffer.
		const GpuBuffer& GetVertexBuffer() const;

		/// @return Buffer con los índices en GPU.
		/// @pre Debe haber sido establecido con SetIndexBuffer.
		const GpuBuffer& GetIndexBuffer() const;


		/// @brief Configura el modelo para que use un único LOD.
		void EstablisSingleLod();

		/// @brief Registra un nuevo LOD.
		/// Será de un nivel de detalle inferior al último nivel registrado.
		/// En caso de registrarse por primera vez, representa el nivel de detalle más
		/// alto.
		/// @param lod LOD a registrar.
		void AddLod(const Lod& lod);


		/// @param lod Índice de nivel (más alto = menos detalle).
		/// @return LOD.
		/// @pre @p lod < número de niveles.
		const Lod& GetLod(UIndex64 lod) const;

		/// @param lod Índice de nivel (más alto = menos detalle).
		/// @return LOD.
		/// En caso de que @p lod > número de niveles, devuelve el nivel  
		/// más alto (menor detalle).
		const Lod& GetLodOrMin(UIndex64 lod) const;


		/// @return Número total de índices del modelo.
		USize32 GetTotalIndexCount() const;

		/// @return Tabla con las texturas usadas por los materiales.
		TextureTable& GetTextureTable();
		const TextureTable& GetTextureTable() const;

		/// @return Mapa con los atributos de los vértices,
		/// almacenados en memoria RAM.
		VerticesAttributesMaps& GetVertexAttributesMap();
		const VerticesAttributesMaps& GetVertexAttributesMap() const;


		/// @return True si tiene un animador 3D.
		bool HasAnimator() const;

		/// @pre `HasAnimator()` debe devolver `true`.
		/// @return Animador.
		Animator& GetAnimator();
		const Animator& GetAnimator() const;

		/// @brief Establece el animador del modelo.
		/// @param animator Animador del modelo.
		void SetAnimator(Animator&& animator);

	private:

		GpuModelUuid m_uuid = GpuModelUuid::CreateEmpty();

		VerticesAttributesMaps m_vertexAttributes{};

		DynamicArray<Lod> m_levelsOfDetail;

		DynamicArray<Material> m_materials;
		TextureTable m_textures;

		DynamicArray<GpuMesh3D> m_meshes{};

		USize32 m_indexCount = 0;

		UniquePtr<GpuBuffer> m_vertexBuffer{};
		UniquePtr<GpuBuffer> m_indexBuffer{};

		CpuModel3D m_cpuModel{};

		std::optional<Animator> m_animator;

	};

}

OSK_DEFINE_UUID_HASH(OSK::GRAPHICS::GpuModelUuid)
