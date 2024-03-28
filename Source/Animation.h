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


	/// @brief Animación para un modelo 3D.
	class OSKAPI_CALL Animation {

	public:

		/// @brief Crea una animación.
		/// @param name Nombre de la animación.
		/// @param samplers Samplers de la animación.
		/// @param channels Canales de la animación.
		/// @param bones Huesos afectados por la animación.
		Animation(
			const std::string& name,
			const DynamicArray<AnimationSampler>& samplers,
			const DynamicArray<AnimationChannel>& channels,
			const DynamicArray<AnimationBone>& bones);

		/// @brief Ejecuta la animación.
		/// @param deltaTime Tiempo transcurrido desde la última actualización.
		/// @param skin Skin de la animación.
		void Update(
			TDeltaTime deltaTime,
			const AnimationSkin& skin);

		/// @brief Establece si la animación debe repetirse después de finalizar.
		/// @param isLooping True si debe repetirse en bucle.
		void SetLooping(bool isLooping);

		/// @return Nombre de la animación.
		std::string_view GetName() const;

		/// @return Esqueleto afectado por la animación.
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
