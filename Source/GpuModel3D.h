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

	/// @brief Identificador �nico para un modelo de GPU.
	using GpuModelUuid = BaseUuid<class GpuModelUuidTag>;

	/// @brief Modelo para renderizado 3D.
	class OSKAPI_CALL GpuModel3D {

	public:

		/// @brief Indica los meshes pertenecientes al modelo
		/// en un nivel de detalle en concreto.
		struct Lod {

			/// @brief Nivel LOD m�s alto posible.
			constexpr static UIndex64 HighestLevel = 0;

			/// @brief �ndice del primer mesh del LOD.
			UIndex64 firstMeshId = 0;

			/// @brief N�mero de meshes del LOD.
			USize64  meshesCount = 0;

		};


		/// @brief Informaci�n del material que puede tener un modelo.
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


		/// @brief Almacena referencias a las texturas o im�genes que
		/// usar� un material de un modelo 3D.
		class TextureTable {

		public:

			/// @param imageIndex �ndice de la imagen.
			/// @return Image view de la imagen indicada.
			/// @pre @p imageIndex debe corresponderse con una imagen existente.
			const IGpuImageView* GetImageView(UIndex64 imageIndex) const;


			/// @brief Establece la textura en el �ndice dado.
			/// @param index �ndice de la imagen.
			/// @param texture Textura a enlazar.
			/// @note No actualiza el �ndice interno usado por
			/// AddTexture y AddGpuImageView.
			void SetTexture(
				UIndex64 index,
				ASSETS::AssetRef<ASSETS::Texture> texture);

			/// @brief Establece la imagen en el �ndice dado.
			/// @param index �ndice de la imagen.
			/// @param view Imagen a enlazar.
			/// @note No actualiza el �ndice interno usado por
			/// AddTexture y AddGpuImageView.
			void SetGpuImageView(
				UIndex64 index,
				const IGpuImageView* view);


			/// @brief A�ade una nueva textura a la tabla.
			/// Comienza en el �ndice 0, y cada llamada aumenta en 
			/// 1 el siguiente �ndice.
			/// @param texture Textura a enlazar.
			/// @note Puede sobreescribir im�genes enlazadas con
			/// SetTexture o SetGpuImageView.
			void AddTexture(ASSETS::AssetRef<ASSETS::Texture> texture);

			/// @brief A�ade una nueva textura a la tabla.
			/// Comienza en el �ndice 0, y cada llamada aumenta en 
			/// 1 el siguiente �ndice.
			/// @param texture Textura a enlazar.
			/// @note Puede sobreescribir im�genes enlazadas con
			/// SetTexture o SetGpuImageView.
			void AddGpuImageView(const IGpuImageView* view);

		private:

			UIndex64 m_nextIndex = 0;

			std::unordered_map<UIndex64, ASSETS::AssetRef<ASSETS::Texture>> m_textures;
			std::unordered_map<UIndex64, const IGpuImageView*> m_imageViews;

		};

	public:

		/// @brief Establece el identificador �nico del modelo.
		/// @param uuid Indentificador �nico.
		void SetUuid(GpuModelUuid uuid);

		/// @return Identificador �nico de este modelo.
		GpuModelUuid GetUuid() const;


		/// @brief Establece el CpuModel3D a partir del cual se ha generado
		/// el modelo 3D.
		/// @param model modelo original.
		void SetCpuModel3D(const CpuModel3D& model);


		/// @return Modelo almacenado en CPU.
		/// Si no se ha establecido mediante SetCpuModel3D,
		/// devuelve un modelo vac�o.
		CpuModel3D& GetCpuModel();

		/// @return Modelo almacenado en CPU.
		/// Si no se ha establecido mediante SetCpuModel3D,
		/// devuelve un modelo vac�o.
		const CpuModel3D& GetCpuModel() const;


		/// @brief Establece el buffer con los v�rtices del modelo.
		/// @param vertexBuffer Buffer con los v�rtices del modelo.
		void SetVertexBuffer(UniquePtr<GpuBuffer>&& vertexBuffer);

		/// @brief Establece el buffer con los �ndices del modelo.
		/// @param vertexBuffer Buffer con los �ndices del modelo.
		void SetIndexBuffer(UniquePtr<GpuBuffer>&& indexBuffer);

		/// @brief Establece el n�mero de �ndices total del modelo.
		/// @param indexCount N�mero de �ndices.
		void SetIndexCount(USize32 indexCount);


		/// @brief A�ade una malla al modelo.
		/// @param mesh Malla.
		void AddMesh(GpuMesh3D&& mesh);

		/// @return Mallas del modelo.
		DynamicArray<GpuMesh3D>& GetMeshes();
		/// @return Mallas del modelo.
		const DynamicArray<GpuMesh3D>& GetMeshes() const;


		void SetMaterials(const DynamicArray<Material>& materials);

		/// @param materialIndex �ndice del material.
		/// @return Material con el �ndice dado.
		/// 
		/// @pre Debe haberse establecido un CpuModel3D mediante SetCpuModel3D.
		/// @pre Debe existir un material con el �ndice indicado.
		Material& GetMaterial(UIndex64 materialIndex);

		/// @param materialIndex �ndice del material.
		/// @return Material con el �ndice dado.
		/// 
		/// @pre Debe haberse establecido un CpuModel3D mediante SetCpuModel3D.
		/// @pre Debe existir un material con el �ndice indicado.
		const Material& GetMaterial(UIndex64 materialIndex) const;


		/// @return Buffer con los v�rtices en GPU.
		/// @pre Debe haber sido establecido con SetVertexBuffer.
		const GpuBuffer& GetVertexBuffer() const;

		/// @return Buffer con los �ndices en GPU.
		/// @pre Debe haber sido establecido con SetIndexBuffer.
		const GpuBuffer& GetIndexBuffer() const;


		/// @brief Configura el modelo para que use un �nico LOD.
		void EstablisSingleLod();

		/// @brief Registra un nuevo LOD.
		/// Ser� de un nivel de detalle inferior al �ltimo nivel registrado.
		/// En caso de registrarse por primera vez, representa el nivel de detalle m�s
		/// alto.
		/// @param lod LOD a registrar.
		void AddLod(const Lod& lod);


		/// @param lod �ndice de nivel (m�s alto = menos detalle).
		/// @return LOD.
		/// @pre @p lod < n�mero de niveles.
		const Lod& GetLod(UIndex64 lod) const;

		/// @param lod �ndice de nivel (m�s alto = menos detalle).
		/// @return LOD.
		/// En caso de que @p lod > n�mero de niveles, devuelve el nivel  
		/// m�s alto (menor detalle).
		const Lod& GetLodOrMin(UIndex64 lod) const;


		/// @return N�mero total de �ndices del modelo.
		USize32 GetTotalIndexCount() const;


		/// @return Tabla con las texturas usadas por los materiales.
		TextureTable& GetTextureTable();
		const TextureTable& GetTextureTable() const;

		/// @return Mapa con los atributos de los v�rtices,
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
