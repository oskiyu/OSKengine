#pragma once

#include <vulkan/vulkan.h>
#include <queue>
#include "Texture.h"
#include "Sprite.h"
#include "Font.h"
#include "TextRenderingLimitActionsEnum.h"
#include "AnchorEnum.h"
#include "AnchorTextToEnum.h"

#include "SpriteContainer.h"
#include "ArrayQueue.hpp"

namespace OSK {

	class Camera2D;

	/// <summary>
	/// Clase que permite renderizar sprites.
	/// </summary>
	class OSKAPI_CALL SpriteBatch {

		friend class RenderAPI;
		friend class RenderSystem3D;
	
	public:

		/// <summary>
		/// Crea un spritebatch vac�o.
		/// </summary>
		SpriteBatch();

		/// <summary>
		/// Destruye el spritebatch.
		/// </summary>
		~SpriteBatch();

		void Begin(Camera2D* camera);
		void End();

		/// <summary>
		/// Dibuja un sprite.
		/// </summary>
		/// <param name="sprite">Sprite a renderizar.</param>
		void DrawSprite(const Sprite& sprite);

		/// <summary>
		/// Renderiza una textura en dos dimensiones.
		/// </summary>
		/// <param name="texture">Textura.</param>
		/// <param name="position">Posici�n en el mundo 2D.</param>
		/// <param name="size">Tama�o en el mundo 2D.</param>
		/// <param name="texCoords">Coordenadas de la textura.</param>
		/// <param name="color">Color.</param>
		void DrawTexture(Texture* texture, const Vector2f position, const Vector2f size, const Vector4i texCoords = { 1 }, const Color& color = { 1.0f });

		/// <summary>
		/// Dibuja un texto.
		/// </summary>
		/// <param name="fuente">Fuente del texto.</param>
		/// <param name="texto">Texto que se renderiza.</param>
		/// <param name="size">Escala (en %) del texto respecto al tama�o de la fuente (1.0f = 100% del tama�o original).</param>
		/// <param name="position">Posici�n en la que se renderiza el texto, en p�xeles desde el ancla ('screenAnchor'), por defecto desde la esquina inferior izquierda de la pantalla.</param>
		/// <param name="color">Color del texto.</param>
		/// <param name="screenAnchor">Establece el origen de coordenadas desde el que se calcula la posici�n del texto.</param>
		/// <param name="reference">Rect�ngulo que indica el �rea sobre la cual se puede renderizar el texto. Por defecto, tama�o de la ventana.</param>
		/// <param name="limitAction">Establece que ocurre cuando el texto sale del �rea.</param>
		/// <param name="sizeXlimit">Si > 0, establece, en p�xeles, el l�mite sobre el cual se aplica.</param>
		/// <param name="limitOffset">Reduce el �rea �til.</param>
		void DrawString(const Font* fuente, const std::string& texto, float size, const Vector2& position, const Color& color = Color(1.0f), Anchor screenAnchor = Anchor::TOP_LEFT, const Vector4& reference = Vector4(-1.0f), TextRenderingLimit limitAction = TextRenderingLimit::DO_NOTHING, float sizeXlimit = 0, float limitOffset = 10);
		
		/// <summary>
		/// Establece la c�mara con la cual se va a renderizar.
		/// </summary>
		/// <param name="camera">C�mrara 2D.</param>
		void SetCamera(Camera2D* camera);

		/// <summary>
		/// Vac�a el spriteBatch.
		/// </summary>
		void Clear();

	private:

		/// <summary>
		/// Obtiene la posici�n final de un texto a renderizar.
		/// </summary>
		/// <param name="position">Posici�n original del texto.</param>
		/// <param name="textSize">Tama�o que ocupa todo el texto.</param>
		/// <param name="anchor">Punto de anclaje.</param>
		/// <param name="to">Elemento sobre el cual se ancla.</param>
		/// <param name="reference">Posici�n real del anclaje.</param>
		/// <returns>Posici�n sobre la cual se empeiza a renderizar el texto.</returns>
		Vector2 GetTextPosition(const Vector2& position, const Vector2& textSize, const Anchor& anchor, const AnchorTextTo& to, const Vector4& reference = Vector4(0.0f)) const;

		/// <summary>
		/// Renderizador.
		/// </summary>
		RenderAPI* renderer = nullptr;

		/// <summary>
		/// Sprites a renderizar.
		/// </summary>
		std::vector<SpriteContainer> spritesToDraw{};

		/// <summary>
		/// C�mara 2D.
		/// </summary>
		Camera2D* camera = nullptr;

		enum class SpriteBatchState {
			STARTED,
			ENDED
		} currentState;

	};

}
