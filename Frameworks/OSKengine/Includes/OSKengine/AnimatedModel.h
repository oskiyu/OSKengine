#pragma once

#include "Model.h"

#include <map>
#include <assimp\anim.h>
#include <assimp\scene.h>
#include <gtc/type_ptr.hpp>

#include "SAnimation.h"
#include "VertexBoneData.h"
#include "BoneInfo.h"

#include "Memory.h"
#include <vector>

namespace OSK {

	/// <summary>
	/// Representa un modelo 3D con animaciones.
	/// </summary>
	class OSKAPI_CALL AnimatedModel : public Model {

		friend class ContentManager;

	public:

		/// <summary>
		/// Destruye el modelo.
		/// </summary>
		~AnimatedModel();

		/// <summary>
		/// Velocidad de la animación.
		/// </summary>
		float_t animationSpeed = 0.75f;
		
		/// <summary>
		/// Establece la animación del modelo.
		/// </summary>
		/// <param name="animID">ID de la nueva animación.</param>
		void SetAnimation(uint32_t animID);

		/// <summary>
		/// Establece la animación del modelo.
		/// </summary>
		/// <param name="name">Nombre de la animación.</param>
		void SetAnimation(const std::string& name);

		/// <summary>
		/// Actualiza el modelo 3D (lo anima).
		/// </summary>
		/// <param name="deltaTime">Delta.</param>
		void Update(float deltaTime);

		/// <summary>
		/// Convierte una matriz formato Assimp a una matriz formato GLM.
		/// </summary>
		/// <param name="from"></param>
		/// <returns></returns>
		static glm::mat4 AiToGLM(const aiMatrix4x4& from);

		/// <summary>
		/// Carga el modelo animado.
		/// </summary>
		void SetupAnimationIndices();

	private:

		/// <summary>
		/// Carga el modelo animado. Recursivo.
		/// </summary>
		/// <param name="node">Nodo padre.</param>
		void SetupAnimationIndices(OSK::Animation::SNode* node);

		/// <summary>
		/// Devuelve el nodo animado buscado.
		/// </summary>
		/// <param name="animation">Animación en la que se va a buscar.</param>
		/// <param name="nodeName">Nombre del nodo.</param>
		/// <returns>Nodo (vacío si no se encuentra).</returns>
		OSK::Animation::SNodeAnim FindNodeAnim(const OSK::Animation::SAnimation* animation, const std::string& nodeName);

		/// <summary>
		/// Obtiene la matriz de posición interpolada para el tiempo dado.
		/// </summary>
		/// <param name="time">Tiempo de la animación.</param>
		/// <param name="node">Nodo del que se busca la posición.</param>
		/// <returns>Matriz de posición</returns>
		glm::mat4 GetPosition(float time, const OSK::Animation::SNodeAnim& node) const;

		/// <summary>
		/// Obtiene la matriz de rotación interpolada para el tiempo dado.
		/// </summary>
		/// <param name="time">Tiempo de la animación.</param>
		/// <param name="node">Nodo del que se busca la posición.</param>
		/// <returns>Matriz de posición</returns>
		glm::mat4 GetRotation(float time, const OSK::Animation::SNodeAnim& node) const;

		/// <summary>
		/// Obtiene la matriz de escala interpolada para el tiempo dado.
		/// </summary>
		/// <param name="time">Tiempo de la animación.</param>
		/// <param name="node">Nodo del que se busca la posición.</param>
		/// <returns>Matriz de posición</returns>
		glm::mat4 GetScale(float time, const OSK::Animation::SNodeAnim& node) const;

		/// <summary>
		/// Tiempo de la animación.
		/// </summary>
		deltaTime_t time = 0.0f;

		/// <summary>
		/// Interpola dos vectores.
		/// </summary>
		/// <param name="vec1">Vector 1.</param>
		/// <param name="vec2">Vector 2.</param>
		/// <param name="delta">Coeficiente de interpolación.</param>
		/// <returns>Vector interpolado.</returns>
		Vector3f InterpolateVectors(const Vector3f& vec1, const Vector3f& vec2, float delta) const;

		/// <summary>
		/// Actualiza todos los huesos de la animación.
		/// Recursivo.
		/// </summary>
		/// <param name="animTime">Tiempo de la animación.</param>
		/// <param name="animation">Nodo.</param>
		/// <param name="parent">Matriz padre.</param>
		void ReadNodeHierarchy(float animTime, OSK::Animation::SNode& animation, const glm::mat4& parent);
		
		/// <summary>
		/// Map nombre del hueso -> hueso.
		/// </summary>
		std::map<std::string, uint32_t> boneMapping;

		/// <summary>
		/// Huesos del esqueleto.
		/// </summary>
		std::vector<BoneInfo> boneInfos;

		/// <summary>
		/// Transformación base de la animación.
		/// </summary>
		glm::mat4 globalInverseTransform;

		/// <summary>
		/// Información sobre que huesos tiene cada vértice.
		/// </summary>
		std::vector<VertexBoneData> bones;

		/// <summary>
		/// Animaciones disponibles.
		/// </summary>
		std::vector<OSK::Animation::SAnimation> animations;

		/// <summary>
		/// Animación actual.
		/// </summary>
		OSK::Animation::SAnimation* currentAnimation = nullptr;

		/// <summary>
		/// Nodo padre final.
		/// </summary>
		OSK::Animation::SNode rootNode;
			

	};

}