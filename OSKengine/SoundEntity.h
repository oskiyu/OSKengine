#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Transform.h"

#include <glm.hpp>

namespace OSK {

	//Clase que almacena un sonido para su reproducción con AudioAPI.
	class OSKAPI_CALL SoundEntity {

	public:

		//Crea un sonido.
		SoundEntity();


		//Destruye el sonido.
		~SoundEntity();


		//INTERNO.
		//NO USAR.
		//Usar Transform.
		//Establece la posiión 3D del sonido.
		void SetPosition(const Vector3& position);


		//Establece la dirección del sonido.
		void SetDirection(const Vector3& direction);


		//Establece la velocidad a la que se mueve el sonido en el mundo 3D.
		void SetSpeed(const Vector3& speed);


		//Establece el tono (agudo/grave) del sonido.
		void SetPitch(const float_t& pitch);


		//Estable la ganancia del sonido.
		void SetGain(const float_t& gain);


		//Transform del sonido.
		//-Posicion.
		Transform SoundTransform;


		//OpenAL.
		OSK_INFO_DO_NOT_TOUCH
			uint32_t BufferID;


		//OpenAL.
		OSK_INFO_DO_NOT_TOUCH
			uint32_t SourceID;

	};

}
