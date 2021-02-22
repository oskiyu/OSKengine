#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Transform.h"

#include <glm.hpp>

#include "ECS.h"

namespace OSK {

	//Clase que almacena un sonido para su reproducci�n con AudioSystem.
	class OSKAPI_CALL SoundEmitterComponent : Component {

	public:

		void OnCreate() override;
		void OnRemove() override;

		//INTERNO.
		//NO USAR.
		//Usar Transform.
		//Establece la posii�n 3D del sonido.
		void SetPosition(const Vector3& position);

		//Establece la direcci�n del sonido.
		void SetDirection(const Vector3& direction);

		//Establece la velocidad a la que se mueve el sonido en el mundo 3D.
		void SetSpeed(const Vector3& speed);

		//Establece el tono (agudo/grave) del sonido.
		void SetPitch(float pitch);

		//Estable la ganancia del sonido.
		void SetGain(float gain);

		//OpenAL.
		uint32_t BufferID;
		//OpenAL.
		uint32_t SourceID;

	};

}
