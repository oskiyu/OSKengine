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

	//Clase para la reproducci�n de sonidos.
	class OSKAPI_CALL AudioSystem : public ECS::System {

	public:

		void OnCreate() override;
		void OnTick(deltaTime_t deltaTime) override;
		void OnRemove() override;

		//Establece la c�mara como entidad 'listener' del AudioSystem.
		void SetCamera3D(Camera3D* camera);

		//Establece la velocidad a la que se mueve la entidad 'listener' (la c�mara).
		//Se usa para simular el movimiento del jugador.
		void SetListenerSpeed(const Vector3& speed);

		//Hace sonar un sonido.
		//<bucle>: si es 'true' el sonido se repite siempre.
		void PlayAudio3D(SoundEmitterComponent& audio, bool bucle = false);

		//Hace sonar un sonido en su posici�n 3D.
		//<bucle>: si es 'true' el sonido se repite siempre.
		void PlayAudio(SoundEmitterComponent& audio, bool bucle = false);
		
		//Pausa el audio dado.
		void PauseAudio(const SoundEmitterComponent& audio);

		//Detiene el audio dado, devolvi�ndolo al segundo 0.
		//void StopAudio(const SoundEmitterComponent& audio);

		//Reinicia el sonido.
		void RestartAudio(const SoundEmitterComponent& audio);

	private:

		ALCdevice* device = nullptr;
		ALCcontext* context = nullptr;

		Camera3D* camera = nullptr;

	};

}
