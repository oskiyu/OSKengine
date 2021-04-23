#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "RenderTarget.h"
#include "SpriteBatch.h"
#include "RenderizableScene.h"

#include <list>

namespace OSK {

	/// <summary>
	/// Representa una unidad de renderizado:
	/// contiene una escena 3D y una serie de spritebatches 2D.
	/// Puede usarse para establecer el orden de renderizado de distintos elementos de la escena final,
	/// o para renderizar una image una sola vez.
	/// </summary>
	class OSKAPI_CALL RenderStage {

		friend class RenderAPI;
		friend class RenderSystem3D;

	public:
		
		/// <summary>
		/// Crea un renderstage vacío.
		/// </summary>
		RenderStage();

		/// <summary>
		/// Destruye el render stage.
		/// </summary>
		~RenderStage();

		/// <summary>
		/// Establece la escena que se va a renderizar.
		/// </summary>
		/// <param name="scene">Escena.</param>
		void SetScene(RenderizableScene* scene);

		/// <summary>
		/// Establece el render target sobre el que se va a renderizar.
		/// </summary>
		/// <param name="rtarget">Render target.</param>
		/// <param name="transferOwnership">True si este stage es dueño del render target.</param>
		void SetRenderTarget(RenderTarget* rtarget, bool transferOwnership);

		/// <summary>
		/// Añade un spritebatch.
		/// </summary>
		/// <param name="sprtBatch">Spritebatch a renderizar.</param>
		void AddSpriteBatch(SpriteBatch* sprtBatch);

		/// <summary>
		/// Elimina un spritebatch.
		/// Si este spritebatch no se encuentra aquí, no ocurrirá nada.
		/// </summary>
		/// <param name="sprtBatch">Spritebatch a eliminar.</param>
		void RemoveSpriteBatch(SpriteBatch* sprtBatch);

		/// <summary>
		/// Quita todos los spritebatches.
		/// </summary>
		void RemoveAllSpriteBatches();

	private:

		/// <summary>
		/// Elimina el renedrtarget, si es suyo.
		/// </summary>
		void Clear();

		/// <summary>
		/// True si es owner del rendertarget.
		/// </summary>
		bool isOwnerOfRTarget = false;

		/// <summary>
		/// Render target.
		/// </summary>
		RenderTarget* RTarget = nullptr;

		/// <summary>
		/// Spritebatches a renderizar.
		/// </summary>
		std::list<SpriteBatch*> SpriteBatches;

		/// <summary>
		/// Escena que se va a renderizar.
		/// </summary>
		RenderizableScene* Scene = nullptr;

	};

}