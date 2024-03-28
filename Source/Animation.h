#pragma once

#include "OSKmacros.h"

#include "DynamicArray.hpp"

#include "Bone.h"
#include "AnimationChannel.h"
#include "AnimationSampler.h"
#include "Skeleton.h"

#include <limits>

namespace OSK::GRAPHICS {

	struct AnimationSkin;


	/// @brief Animaci�n para un modelo 3D.
	class OSKAPI_CALL Animation {

	public:

		/// @brief Crea una animaci�n.
		/// @param name Nombre de la animaci�n.
		/// @param samplers Samplers de la animaci�n.
		/// @param channels Canales de la animaci�n.
		/// @param bones Huesos afectados por la animaci�n.
		Animation(
			const std::string& name,
			const DynamicArray<AnimationSampler>& samplers,
			const DynamicArray<AnimationChannel>& channels,
			const DynamicArray<AnimationBone>& bones);

		/// @brief Ejecuta la animaci�n.
		/// @param deltaTime Tiempo transcurrido desde la �ltima actualizaci�n.
		/// @param skin Skin de la animaci�n.
		void Update(
			TDeltaTime deltaTime,
			const AnimationSkin& skin);

		/// @brief Establece si la animaci�n debe repetirse despu�s de finalizar.
		/// @param isLooping True si debe repetirse en bucle.
		void SetLooping(bool isLooping);

		/// @return Nombre de la animaci�n.
		std::string_view GetName() const;

		/// @return Esqueleto afectado por la animaci�n.
		const Skeleton& GetSkeleton() const;

	private:

		std::string m_name;

		Skeleton m_skeleton;

		DynamicArray<AnimationSampler> m_samplers;
		DynamicArray<AnimationChannel> m_channels;

		TDeltaTime m_startTime = 0.0f;
		TDeltaTime m_endTime = 0.0f;

		TDeltaTime m_currentTime = 0.0f;

		bool m_shouldLoop = true;

	};

}
