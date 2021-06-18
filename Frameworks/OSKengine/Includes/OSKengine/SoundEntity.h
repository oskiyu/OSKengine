#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Transform.h"

#include <glm.hpp>

#include "ECS.h"

namespace OSK {

	class AudioSystem;

	/// <summary>
	/// Clase que almacena un sonido para su reproducción con AudioSystem.
	/// </summary>
	class OSKAPI_CALL SoundEmitterComponent : Component {

		friend class AudioSystem;
		friend class ContentManager;

	public:

		/// <summary>
		/// Crea el componente.
		/// </summary>
		void OnCreate() override;

		/// <summary>
		/// Cierra el componente.
		/// </summary>
		void OnRemove() override;

		/// <summary>
		/// INTERNO <para/>
		/// NO USAR <para/>
		///			<para/>
		/// Establece la posiión 3D del sonido.
		/// </summary>
		/// <param name="position"></param>
		void SetPosition(const Vector3& position);

		/// <summary>
		/// Establece la dirección del sonido.
		/// </summary>
		/// <param name="direction"></param>
		void SetDirection(const Vector3& direction);

		/// <summary>
		/// Establece la velocidad a la que se mueve el sonido en el mundo 3D.
		/// </summary>
		/// <param name="speed"></param>
		void SetSpeed(const Vector3& speed);

		/// <summary>
		/// Establece el tono (agudo/grave) del sonido.
		/// </summary>
		/// <param name="pitch">Pitch.</param>
		void SetPitch(float pitch);

		/// <summary>
		/// Estable la ganancia del sonido.
		/// </summary>
		/// <param name="gain">Ganancia.</param>
		void SetGain(float gain);

	private:

		/// <summary>
		/// OpenAL.
		/// </summary>
		uint32_t bufferID;

		/// <summary>
		/// OpenAL.
		/// </summary>
		uint32_t sourceID;

	};

}
