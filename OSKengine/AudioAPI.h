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

	//Clase para la reproducci�n de sonidos.
	class OSKAPI_CALL AudioAPI {

	public:

		//Crea una instancia AudioAPI.
		AudioAPI();


		//Destruye el AudioAPI.
		~AudioAPI();


		//Sincronza la posici�n del AudioAPI con la posici�n de la c�mara.
		void Update();


		//Establece la c�mara como entidad 'listener' del AudioAPI.
		void SetCamera3D(OldCamera3D* camera);


		//Establece la velocidad a la que se mueve la entidad 'listener' (la c�mara).
		//Se usa para simular el movimiento del jugador.
		void SetListenerSpeed(const Vector3& speed);


		//Hace sonar un sonido.
		//<bucle>: si es 'true' el sonido se repite siempre.
		void PlayAudio3D(SoundEntity& audio, const bool& bucle = false);


		//Hace sonar un sonido en su posici�n 3D.
		//<bucle>: si es 'true' el sonido se repite siempre.
		void PlayAudio(SoundEntity& audio, const bool& bucle = false);
		

		//Pausa el audio dado.
		void PauseAudio(const SoundEntity& audio);


		OSK_INFO_NOT_DEVELOPED
		//Detiene el audio dado, devolvi�ndolo al segundo 0.
		void StopAudio(const SoundEntity& audio);


		//Reinicia el sonido.
		void RestartAudio(const SoundEntity& audio);

	private:

		ALCdevice* device = nullptr;

		ALCcontext* context = nullptr;

		OldCamera3D* camera = nullptr;

	};

}