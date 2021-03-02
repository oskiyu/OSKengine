#pragma once

#include "Model.h"

#include <map>
#include <assimp\anim.h>
#include <assimp\scene.h>
#include <gtc/type_ptr.hpp>

#include "SAnimation.h"

namespace OSK {

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
		glm::mat4 Offset;

		//Transformación del hueso.
		glm::mat4 FinalTransformation;

	};


	//Representa un modelo 3D con animaciones.
	class OSKAPI_CALL AnimatedModel : public Model {

		friend class ContentManager;

	public:

		~AnimatedModel() {
			for (uint32_t i = 0; i < NumberOfAnimations; i++)
				Animations[i].Clear();

			SafeDeleteArray(&Animations);
		}

		//Diccionario string(nombre del hueso) -> hueso.
		std::map<std::string, uint32_t> BoneMapping;

		//Huesos del esqueleto.
		std::vector<BoneInfo> BoneInfos;
				
		glm::mat4 GlobalInverseTransform;

		//Información sobre que huesos tiene cada vértice.
		std::vector<VertexBoneData> Bones;

		//Velocidad de la animación.
		float_t AnimationSpeed = 0.75f;

		//Animación actual.
		//aiAnimation* Animation;
		//aiScene* scene;
		OSK::Animation::SAnimation* Animations = nullptr;
		uint32_t NumberOfAnimations = 0;
		OSK::Animation::SAnimation* CurrentAnimation = nullptr;

		//Establece la animación activa.
		void SetAnimation(uint32_t animID) {
			//Animation = scene->mAnimations[animID];
			CurrentAnimation = &Animations[animID];
		}

		void SetAnimation(const std::string& name) {
			for (uint32_t i = 0; i < NumberOfAnimations; i++) {
				if (Animations[i].Name == name) {
					CurrentAnimation = &Animations[i];

					return;
				}
			}
		}

		deltaTime_t time = 0.0f;
		//Actualiza el modelo 3D (lo anima).
		void Update(float deltaTime);

		static inline glm::mat4 AiToGLM(const aiMatrix4x4& from) {
			return glm::transpose(glm::make_mat4(&from.a1));
		}

		void SetupAnimationIndices() {
			SetupAnimationIndices(&RootNode);
		}

	private:

		void SetupAnimationIndices(OSK::Animation::SNode* node);

		OSK::Animation::SNodeAnim FindNodeAnim(const OSK::Animation::SAnimation* animation, const std::string& nodeName);

		glm::mat4 GetPosition(float time, const OSK::Animation::SNodeAnim& node) const;
		glm::mat4 GetRotation(float time, const OSK::Animation::SNodeAnim& node) const;
		glm::mat4 GetScale(float time, const OSK::Animation::SNodeAnim& node) const;

		inline Vector3f InterpolateVectors(const Vector3f& vec1, const Vector3f& vec2, float delta) const {
			return vec1 + (vec2 - vec1) * delta;
		}

		void ReadNodeHierarchy(float animTime, OSK::Animation::SNode& animation, const glm::mat4& parent);

		OSK::Animation::SNode RootNode;

	};

}