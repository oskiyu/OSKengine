#pragma once

#include "Model.h"

#include <map>
#include <assimp\anim.h>
#include <assimp\scene.h>

namespace OSK {


	struct OSKAPI_CALL AnimUBO {
		glm::mat4* Bones;
	};


	//Contiene los huesos a los que está ligado un vértice.
	struct OSKAPI_CALL VertexBoneData {
		
		//ID de los huesos.
		std::array<uint32_t, OSK_ANIM_MAX_BONES_PER_VERTEX> IDs;

		//Pesos de los huesos.
		std::array<float_t, OSK_ANIM_MAX_BONES_PER_VERTEX> Weights;

		//Liga un vértice con un hueso.
		void Add(uint32_t id, float_t weight);

	};


	//El propio hueso.
	//Contiene las matrices del hueso.
	struct OSKAPI_CALL BoneInfo {

		BoneInfo();

		//Offset del hueso.
		aiMatrix4x4 Offset;

		//Transformación del hueso.
		aiMatrix4x4 FinalTransformation;

	};


	//Representa un modelo 3D con animaciones.
	class OSKAPI_CALL AnimatedModel : public Model {

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
		void SetAnimation(uint32_t animID) {
			Animation = scene->mAnimations[animID];
		}


		deltaTime_t time = 0.0f;
		//Actualiza el modelo 3D (lo anima).
		void Update(float deltaTime);

	private:

		const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string& nodeName) const;

		aiMatrix4x4 InterpolateTranslation(float time, const aiNodeAnim* node) const;
	
		aiMatrix4x4 InterpolateRotation(float time, const aiNodeAnim* node) const;

		aiMatrix4x4 InterpolateScale(float time, const aiNodeAnim* node) const;

		void ReadNodeHierarchy(float animTime, const aiNode* node, const aiMatrix4x4& parent);

	};

}