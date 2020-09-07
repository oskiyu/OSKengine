#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "AnchorEnum.h"
#include "AnchorTextToEnum.h"
#include "OldCamera3D.h"
#include "Color.h"
#include "OldFont.h"
#include "OldModel.h"
#include "PointLight.h"
#include "RenderModeEnum.h"
#include "RenderOutputImageEnum.h"
#include "RenderSettingsEnum.h"
#include "Shader.h"
#include "OldSkybox.h"
#include "TextRenderingLimitActionsEnum.h"
#include "Transform.h"
#include "WindowAPI.h"

#include <glm.hpp>

namespace OSK {
	
	//Numero máximo de luces puntuales soportadas.
	constexpr uint32_t MAX_LIGHTS = 32;
	
	//Clase para el renderizado.
	class OSKAPI_CALL RenderAPI {

	public:

		//Inicia el RenderAPI.
		RenderAPI();


		//Destruye el RenderAPI.
		~RenderAPI();


		//Inicia el RenderAPI con los ajustes por defecto.
		void Init();


		//Actualiza un ajuste en concreto.
		void UpdateSetting(const RenderSettings& settings, const bool& value);


		//Limpia la pantalla.
		//<color>: color con el que se limpia la pantalla.
		void Clear(const Color& color);
		

		//Establece el shader del renderizado para la escena.
		void SetSceneShader(Shader* shader);


		//Establece el shader del renderizado para la pantalla.
		//Post-procesamiento.
		void SetScreenShader(Shader* shader);


		//Establece el shader del skybox.
		void SetSkyboxShader(Shader* shader);


		//Establece el shader para el renderizado de texto en pantalla.
		void SetTextShader(Shader* shader);


		//Establece el shader para el renderizado 2D.
		void SetSpriteShader(Shader* shader);


		//Establece el skybox a usar.
		void SetSkybox(const OldSkybox& skybox);


		//Establece la cámara desde la que se renderizará un mundo 3D en la pantalla seleccionada.
		void SetCamera3D(OldCamera3D* camera);


		//Renderiza un modelo.
		void DrawModel(OldModel& model, const deltaTime_t& deltaTime);


		//Renderiza un usando el transform dado.
		void DrawModel(OldModel& model, const OSK::Transform& transform, const deltaTime_t& deltaTime);


		//Renderiza un texto.
		//<fuente>: fuente del texto.
		//<texto>: texto que se renderiza.
		//<position>: posición en la que se renderiza el texto, en píxeles desde el ancla (<screenAnchor>), por defecto desde la esquina inferior izquierda de la pantalla.
		//<color>: color del texto.
		//<screenAnchor>: establece el origen de coordenadas desde el que se calcula la posición del texto.
		//<limitAction>: establece que ocurre cuando el texto sale de la pantalla.
		//<sizeXlimit>: si > 0, establece, en píxeles, el límite sobre el cual se aplica <limitAction>.
		void DrawString(OldFont& fuente, const float_t& size, const std::string& texto, const Vector2& position, const Color &color = Color(1.0f, 1.0f, 1.0f), const Anchor& screenAnchor = Anchor::BOTTOM_LEFT, const Vector4& reference = Vector4(-1.0f), const TextRenderingLimit& limitAction = TextRenderingLimit::DO_NOTHING, const float_t& sizeXlimit = 0, const float_t& limitOffset = 10) const;


		//Renderiza una imagen 2D.
		//<texture>: textura que se va a renderizar.
		//<position>: la posición, en píxeles, en la que se va a renderizar la textura, tomando como origen de coordenadas la esquina inferior izquierda de la pantalla.
		//<size>: tamaño al que se va a renderizar la imagen, en píxeles.
		//<color>: color de tinte que se va a aplicar a la imagen.
		//<texCoords>: rectángulo que selecciona la parte de la textura que se va a renderizar, en píxeles. Si texCoords.x > 0: se renderiza la imagen completa.
		//<rotation>: rotación que se le aplica a la textura sobre su centro de rotación (<origin>), en grados.
		//<origin>: centro de rotación, en píxeles desde la esquina inferior derecha de la imagen.
		void DrawTexture(const OldTexture& texture, const Vector2& position, const Vector2& size, const Color& color = Color(1.0f, 1.0f, 1.0f), const Vector4& texCoords = Vector4(-1.0f), const float_t& rotation = 0, const Vector2 origin = Vector2(0.0f, 0.0f)) const;


		void DrawUserInterface();


		void drawUIElement(UI::BaseUIElement* element);


		//Establece la WindowAPI sobre la que se va a renderizar.
		void SetWindowAPI(WindowAPI* window);
		

		//Empieza el primer render-pass.
		void StartFirstRenderPass();


		//Empieza un render-pass para el renderizado de texto.
		void StartTextRenderPass();


		//Empieza un render-pass para el renderizado de texturas 2D.
		void StartSpriteRenderPass();


		//Empieza el segundo render-pass.
		void StartSecondRenderPass();


		//Renderiza el skybox.
		void DrawSkybox();


		//Empieza el segundo render-pass.
		//Post-procesamiento.
		void RenderScreen();


		//Añade una luz puntual.
		void AddPointLight(PointLight* luz);


		//Actualiza el viewport.
		void UpdateViewport();


		//Las luces puntuales del renderizador.
		std::vector<PointLight*> PointLights;


		//Output del renderizado.
		RenderOutput RenderOutputImage;


		//Profundidad.
		bool EnableDepthTest;


		//Stencil.
		bool EnableStencilTest;


		//Optimización del renderizado de triángulos.
		bool EnableCullFace;


		//Transparencia.
		bool EnableBlend;


		//¿Debe cambiar de resolución el Viewport al cambiar de tamaño la panalla?
		bool DynamicResolution = true;


		//Si es true, activa V-Sync.
		bool VerticalSync = true;

	private:

		void enableOrDisableSetting(const OSK::RenderSettings&, const bool&);
		
		Vector2 getTextPosition(const Vector2& position, const Vector2& textSize, const Anchor& anchor, const AnchorTextTo& to, const Vector4& reference = Vector4(0.0f)) const;

		WindowAPI* window = nullptr;

		OSK::OldCamera3D* camera = nullptr;

		Shader* sceneShader = nullptr;

		Shader* screenShader = nullptr;

		OSK::Shader* skyboxShader = nullptr;

		OSK::Shader* textShader = nullptr;

		OSK::Shader* spriteShader = nullptr;

		OSK::OldSkybox skybox;

		bufferObject_t frameBufferObject;

		bufferObject_t framebuffer;

		bufferObject_t renderBufferObject;

		OldTexture renderTexture;

		vertexArrayObject_t quadVAO;

		bufferObject_t quadVBO;

		vertexArrayObject_t skyboxVAO;

		bufferObject_t skyboxVBO;

		vertexArrayObject_t textVAO;

		bufferObject_t textVBO;

		vertexArrayObject_t spriteVAO;

		bufferObject_t spriteVBO;

		const float_t quadVertices[24] = {
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};

		const float_t skyboxVertices[18 * 6] = {
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

	};

}