#pragma once

#include "Model.h"

#include <map>
#include <assimp\anim.h>
#include <assimp\scene.h>

namespace OSK {


	struct AnimUBO {
		glm::mat4* Bones;
	};


	//Contiene los huesos a los que está ligado un vértice.
	struct VertexBoneData {
		
		//ID de los huesos.
		std::array<uint32_t, OSK_ANIM_MAX_BONES_PER_VERTEX> IDs;

		//Pesos de los huesos.
		std::array<float_t, OSK_ANIM_MAX_BONES_PER_VERTEX> Weights;

		//Liga un vértice con un hueso.
		void Add(const uint32_t& id, const float_t& weight);

	};


	//El propio hueso.
	//Contiene las matrices del hueso.
	struct BoneInfo {

		BoneInfo();

		//Offset del hueso.
		aiMatrix4x4 Offset;

		//Transformación del hueso.
		aiMatrix4x4 FinalTransformation;

	};


	//Representa un modelo 3D con animaciones.
	class AnimatedModel : public Model {

	public:

		//Diccionario string(nombre del hueso) -> hueso.
		std::map<std::string, uint32_t> BoneMapping;

		//Huesos del esqueleto.
		std::vector<BoneInfo> BoneInfos;

		//Número de huesos.
		uint32_t NumBones = 0;
		
		aiMatrix4x4 GlobalInverseTransform;

		//Información sobre que huesos tiene cada vértice.
		std::vector<VertexBoneData> Bones;

		//Matrices de los huesos.
		std::vector<aiMatrix4x4> BoneTransforms;

		//Velocidad de la animación.
		float_t AnimationSpeed = 0.75f;

		//Animación actual.
		aiAnimation* Animation;
		const aiScene* scene;

		//Establece la animación activa.
		void SetAnimation(const uint32_t& animID) {
			Animation = scene->mAnimations[animID];
		}


		deltaTime_t time = 0.0f;
		//Actualiza el modelo 3D (lo anima).
		void Update(const float_t& deltaTime);

		const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string& nodeName) const;

		aiMatrix4x4 InterpolateTranslation(const float_t& time, const aiNodeAnim* node) const;
	
		aiMatrix4x4 InterpolateRotation(const float_t& time, const aiNodeAnim* node) const;

		aiMatrix4x4 InterpolateScale(const float_t& time, const aiNodeAnim* node) const;

		void ReadNodeHierarchy(const float_t& animTime, const aiNode* node, const aiMatrix4x4& parent);

	};

}