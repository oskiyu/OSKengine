#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "OldCamera3D.h"
#include "SoundEntity.h"

#include <alc.h>
#include <glm.hpp>

namespace OSK {

	//Clase para la reproducción de sonidos.
	class OSKAPI_CALL AudioAPI {

	public:

		//Crea una instancia AudioAPI.
		AudioAPI();


		//Destruye el AudioAPI.
		~AudioAPI();


		//Sincronza la posición del AudioAPI con la posición de la cámara.
		void Update();


		//Establece la cámara como entidad 'listener' del AudioAPI.
		void SetCamera3D(OldCamera3D* camera);


		//Establece la velocidad a la que se mueve la entidad 'listener' (la cámara).
		//Se usa para simular el movimiento del jugador.
		void SetListenerSpeed(const Vector3& speed);


		//Hace sonar un sonido.
		//<bucle>: si es 'true' el sonido se repite siempre.
		void PlayAudio3D(SoundEntity& audio, const bool& bucle = false);


		//Hace sonar un sonido en su posición 3D.
		//<bucle>: si es 'true' el sonido se repite siempre.
		void PlayAudio(SoundEntity& audio, const bool& bucle = false);
		

		//Pausa el audio dado.
		void PauseAudio(const SoundEntity& audio);


		OSK_INFO_NOT_DEVELOPED
		//Detiene el audio dado, devolviéndolo al segundo 0.
		void StopAudio(const SoundEntity& audio);


		//Reinicia el sonido.
		void RestartAudio(const SoundEntity& audio);

	private:

		ALCdevice* device = nullptr;

		ALCcontext* context = nullptr;

		OldCamera3D* camera = nullptr;

	};

}