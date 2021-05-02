#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "SoundEntity.h"
#include "Camera3D.h"

#include <alc.h>
#include <glm.hpp>

#include "ECS.h"

namespace OSK {

	/// <summary>
	/// Clase para la reproducción de sonidos.
	/// </summary>
	class OSKAPI_CALL AudioSystem : public ECS::System {

	public:

		/// <summary>
		/// Crea el sistema. Se llama automáticamente mediante el sistema ECS.
		/// </summary>
		void OnCreate() override;

		/// <summary>
		/// Actualiza el sistema. Se llama automáticamente mediante el sistema ECS.
		/// </summary>
		/// <param name="deltaTime">Delta.</param>
		void OnTick(deltaTime_t deltaTime) override;

		/// <summary>
		/// Cierra el sistema. Se llama automáticamente mediante el sistema ECS.
		/// </summary>
		void OnRemove() override;

		/// <summary>
		/// Establece la cámara como entidad 'listener' del AudioSystem.
		/// </summary>
		/// <param name="camera"></param>
		void SetCamera3D(Camera3D* camera);

		/// <summary>
		/// Establece la velocidad a la que se mueve la entidad 'listener' (la cámara). <para/>
		/// Se usa para simular el movimiento del jugador.
		/// </summary>
		/// <param name="speed">Velocidad.</param>
		void SetListenerSpeed(const Vector3& speed);

		/// <summary>
		/// Hace sonar un sonido (3D).
		/// </summary>
		/// <param name="audio">Sonido a reproducir.</param>
		/// <param name="bucle">Si es 'true' el sonido se repite siempre.</param>
		void PlayAudio3D(SoundEmitterComponent& audio, bool bucle = false);

		/// <summary>
		/// Hace sonar un sonido.
		/// </summary>
		/// <param name="audio">Sonido a reproducir.</param>
		/// <param name="bucle">Si es 'true' el sonido se repite siempre.</param>
		void PlayAudio(SoundEmitterComponent& audio, bool bucle = false);
		
		/// <summary>
		/// Pausa el audio dado.
		/// </summary>
		/// <param name="audio">Audio a pausar.</param>
		void PauseAudio(const SoundEmitterComponent& audio);

		/// <summary>
		/// Reinicia el sonido.
		/// </summary>
		/// <param name="audio">Sonido a reiniciar.</param>
		void RestartAudio(const SoundEmitterComponent& audio);

	private:

		/// <summary>
		/// INTERNAL: .
		/// </summary>
		ALCdevice* device = nullptr;

		/// <summary>
		/// 
		/// </summary>
		ALCcontext* context = nullptr;

		/// <summary>
		/// Camara para la reproducción 3D.
		/// </summary>
		Camera3D* camera = nullptr;

	};

}
