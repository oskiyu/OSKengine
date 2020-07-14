#include "RenderAPI.h"

#include <glad/glad.h>

#include "ToString.h"
#include "BaseUIElement.h"

namespace OSK {

	RenderAPI::RenderAPI() {

	}


	RenderAPI::~RenderAPI() {
		glDeleteVertexArrays(1, &quadVAO);
		glDeleteBuffers(1, &quadVBO);
	}


	void RenderAPI::Init() {

		PointLights.reserve(MAX_LIGHTS);
#ifdef OSK_USE_INFO_LOGS
		OSK::Logger::Log(LogMessageLevels::INFO, "max luces puntuales: " + std::to_string(PointLights.capacity()));
#endif // OSK_USE_INFO_LOGS

		UpdateSetting(RenderSettings::ENABLE_BLEND, true);
		UpdateSetting(RenderSettings::ENABLE_CULL_FACE, true);
		UpdateSetting(RenderSettings::ENABLE_DEPTH_TEST, true);
		UpdateSetting(RenderSettings::ENABLE_STENCIL_TEST, true);
		UpdateSetting(RenderSettings::DYNAMIC_RES, true);
		UpdateSetting(RenderSettings::V_SYNC, true);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Pantalla.
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenTextures(1, &renderTexture.ID);
		glBindTexture(GL_TEXTURE_2D, renderTexture.ID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window->ScreenSizeX, window->ScreenSizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture.ID, 0);
		
		glGenRenderbuffers(1, &renderBufferObject);
		glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObject);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window->ScreenSizeX, window->ScreenSizeY);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);
		
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			Logger::Log(LogMessageLevels::CRITICAL_ERROR, "FRAMEBUFFER: Framebuffer is not complete", __LINE__);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float_t), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float_t), (void*)(2 * sizeof(float_t)));

		//Sprites.
		glGenVertexArrays(1, &spriteVAO);
		glGenBuffers(1, &spriteVBO);
		glBindVertexArray(spriteVAO);
		glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		//Texto.
		glGenVertexArrays(1, &textVAO);
		glGenBuffers(1, &textVBO);
		glBindVertexArray(textVAO);
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		//Skybox.
		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		RenderOutputImage = OSK::RenderOutput::SCENE;
	}


	void RenderAPI::UpdateSetting(const RenderSettings& settings, const bool& value) {
		enableOrDisableSetting(settings, value);
	}


	void RenderAPI::Clear(const Color& color) {
		glClearColor(color.Red, color.Green, color.Blue, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}


	void RenderAPI::DrawModel(Model& model, const deltaTime_t& deltaTime) {
		DrawModel(model, model.ModelTransform, deltaTime);
	}


	void RenderAPI::DrawModel(Model& model, const Transform& transform, const deltaTime_t& deltaTime) {
		if (sceneShader == nullptr || screenShader->ProgramID == 0) {
			OSK::Logger::Log(LogMessageLevels::BAD_ERROR, "sceneShader no ha sido cargada.", __LINE__);
			return;
		}
		if (!model.IsLoaded) {
			OSK::Logger::Log(LogMessageLevels::BAD_ERROR, "el modelo no ha sido cargado.", __LINE__);
			return;
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, model.Diffuse.ID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, model.Specular.ID);

		sceneShader->SetModel(transform.ModelMatrix);
		if (model.Skeletal) {
			sceneShader->SetBool("Skeleton", true);

			for (uint32_t i = 0; i < MAX_BONES_AMOUNT; i++) {
				glm::mat4 boneModel = glm::mat4(1.0f);

				if (i < model.Skeleton.Bones.size())
					boneModel = model.Skeleton.Bones[i].Transform.ModelMatrix;

				sceneShader->SetMat4("bones[" + std::to_string(i) + "]", boneModel);

			}
		}
		else {
			sceneShader->SetBool("Skeleton", false);
		}

		for (unsigned int i = 0; i < model.Meshes.size(); i++) {
			glBindVertexArray(model.Meshes[i].VAO);
			glDrawElements(GL_TRIANGLES, model.Meshes[i].Indices.size(), GL_UNSIGNED_INT, 0);
		}
	}
	

	void RenderAPI::DrawString(OldFont& fuente, const float_t& size, const std::string& texto, const Vector2& position, const Color& color, const Anchor& screenAnchor, const Vector4& reference,const TextRenderingLimit& limitAction, const float_t& sizeXlimit, const float_t& limitOffset) const {
		if (textShader == nullptr || textShader->ProgramID == 0) {
			OSK::Logger::Log(LogMessageLevels::BAD_ERROR, "textShader no ha sido cargada.", __LINE__);
			return;
		}
		if (!fuente.IsLoaded) {
			OSK::Logger::Log(LogMessageLevels::BAD_ERROR, "la fuente no ha sido cargada.", __LINE__);
			return;
		}
		
		textShader->SetColor("textColor", color);

		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(textVAO);

		AnchorTextTo to = AnchorTextTo::SCREEN;
		if (reference.X > 0.0f)
			to = AnchorTextTo::UI_ELEMENT;

		Vector2 textSize = GetTextSize(texto, fuente, size);
		Vector2 finalPosition = getTextPosition(position, textSize, screenAnchor, to, reference);
		
		std::string finalText = texto;
		
		if (limitAction == OSK::TextRenderingLimit::MOVE_TEXT) {
			if (finalPosition.X + textSize.X + limitOffset > window->ScreenSizeX) {
				finalPosition.X -= limitOffset + (finalPosition.X + textSize.X - window->ScreenSizeX);
			}
		}
		else if (limitAction == TextRenderingLimit::NEW_LINE) {
			float_t textSizeX = 0.0f;

			for (auto c = texto.begin(); c != texto.end(); c++) {
				OldFontChar character = fuente.Characters[*c];
				if (*c == '\n') {
					textSizeX = 0;
				}
				else if (*c == '\t') {
					textSizeX += (character.Bearing.x * size + character.Size.x * size) * 3;
				}
				else {
					textSizeX += character.Bearing.x * size + character.Size.x * size;
				}

				if (textSizeX + finalPosition.X + limitOffset > window->ScreenSizeX) {
					finalText.insert(c - 1, '\n');
					textSize = 0;
				}
			}
		}

		//Render:
		float_t x = finalPosition.X;
		float_t y = finalPosition.Y;
		for (auto c = finalText.begin(); c != finalText.end(); c++) {

			OldFontChar character = fuente.Characters[*c];
			if (*c == '\n') {
				y -= character.Size.y * size + character.Bearing.y;
				x = finalPosition.X;

				continue;
			}

			if (*c == '\t') {
				x += x += (character.Advance >> 6)* size * SPACES_PER_TAB;

				continue;
			}

			if (*c == ' ') {
				x += (character.Advance >> 6)* size;

				continue;
			}

			GLfloat posX = x + character.Bearing.x * size;
			GLfloat posY = y - (character.Size.y - character.Bearing.y) * size;

			GLfloat sizeX = character.Size.x * size;
			GLfloat sizeY = character.Size.y * size;

			GLfloat vertices[6][4] = {
				{ posX, posY + sizeY, 0, 0 },
				{ posX, posY, 0, 1 },
				{ posX + sizeX, posY, 1, 1 },

				{ posX, posY + sizeY, 0, 0 },
				{ posX + sizeX, posY, 1, 1 },
				{ posX + sizeX, posY + sizeY, 1, 0 }
			};

			glBindTexture(GL_TEXTURE_2D, character.ID);

			glBindBuffer(GL_ARRAY_BUFFER, textVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			x += (character.Advance >> 6)* size;
		}

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	void RenderAPI::DrawTexture(const OldTexture& texture, const Vector2& position, const Vector2& size, const Color& color, const Vector4& texCoords, const float_t& rotation, const Vector2 origin) const {
		if (spriteShader == nullptr || spriteShader->ProgramID == 0) {
			Logger::Log(LogMessageLevels::BAD_ERROR, "spriteShader no ha sido cargado.", __LINE__);
			return;
		}
		if (!texture.IsLoaded) {
			Logger::Log(LogMessageLevels::BAD_ERROR, "la textura no ha sido cargada.", __LINE__);
			return;
		}
		
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(spriteVAO);

		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, OSK::Vector3((origin.X + position.X + size.X), (origin.Y + position.Y + size.Y), 0.0f).ToGLM());
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, OSK::Vector3(-(origin.X + position.X + size.X), -(origin.Y + position.Y + size.Y), 0).ToGLM());
		spriteShader->SetModel(model);
		spriteShader->SetColor("textColor", color);

		Vector4 coords = texCoords;
		if (texCoords.X < 0) {
			coords = Vector4(0.0f, 0.0f, texture.Size.X, texture.Size.Y);
		}

		GLfloat vertices[6][4] = {
				{ position.X, position.Y + size.Y, coords.X / texture.Size.X, coords.Y / texture.Size.Y },
				{ position.X, position.Y, coords.X / texture.Size.X, coords.W / texture.Size.Y },
				{ position.X + size.X, position.Y, coords.Z / texture.Size.X, coords.W / texture.Size.Y },

				{ position.X, position.Y + size.Y, coords.X / texture.Size.X, coords.Y / texture.Size.Y },
				{ position.X + size.X, position.Y, coords.Z / texture.Size.X, coords.W / texture.Size.Y },
				{ position.X + size.X, position.Y + size.Y, coords.Z / texture.Size.X, coords.Y / texture.Size.Y }
		};

		glBindTexture(GL_TEXTURE_2D, texture.ID);

		glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}


	void RenderAPI::DrawUserInterface() {
		if (window == nullptr)
			return;
		if (window->UserInterface == nullptr || !window->UserInterface->IsActive || !window->UserInterface->IsVisible)
			return;

		window->UserInterface->Draw(*this);
	}


	void RenderAPI::SetSceneShader(Shader* shader) {
		sceneShader = shader;
	}


	void RenderAPI::SetScreenShader(Shader* shader) {
		screenShader = shader;
	}


	void RenderAPI::SetSkyboxShader(Shader* shader) {
		skyboxShader = shader;
	}


	void RenderAPI::SetTextShader(OSK::Shader* shader) {
		textShader = shader;
	}
	

	void RenderAPI::SetSpriteShader(OSK::Shader* shader) {
		spriteShader = shader;
	}


	void RenderAPI::SetSkybox(const Skybox& skybox) {
		this->skybox = skybox;
	}


	void RenderAPI::SetCamera3D(Camera3D* camera) {
		this->camera = camera;
	}


	void RenderAPI::SetWindowAPI(WindowAPI* window) {
		this->window = window;

		glfwSwapInterval(static_cast<int32_t>(VerticalSync));
	}


	void RenderAPI::StartFirstRenderPass() {
		if (window == nullptr) {
			OSK::Logger::Log(LogMessageLevels::BAD_ERROR, "StartFirstRenderPass: window es nullptr.", __LINE__);
			return;
		}
		if (sceneShader == nullptr || sceneShader->ProgramID == 0) {
			OSK::Logger::Log(LogMessageLevels::BAD_ERROR, "sceneShader no ha sido cargado.", __LINE__);
			return;
		}
		if (camera == nullptr) {
			OSK::Logger::Log(LogMessageLevels::BAD_ERROR, "StartFirstRenderPass: camera es nullptr.", __LINE__);
			return;
		}

		if (window->ViewportShouldBeUpdated) {
			UpdateViewport();
			window->ViewportShouldBeUpdated = false;
		}

		sceneShader->Use();
		sceneShader->SetView(camera->GetView());
		sceneShader->SetProjection(camera->GetProjection());
		sceneShader->SetVec3("viewPos", camera->CameraTransform.GlobalPosition);

		sceneShader->SetInt32("NumberOfPointLights", PointLights.size());
		for (uint32_t i = 0; i < PointLights.size(); i++) {
			sceneShader->SetVec3("pointLights[" + std::to_string(i) + "].Position", PointLights[i]->LightTransform.GlobalPosition);
			sceneShader->SetColor("pointLights[" + std::to_string(i) + "].Color", PointLights[i]->Color);
			sceneShader->SetFloat("pointLights[" + std::to_string(i) + "].Constant", PointLights[i]->Constant);
			sceneShader->SetFloat("pointLights[" + std::to_string(i) + "].Linear", PointLights[i]->Linear);
			sceneShader->SetFloat("pointLights[" + std::to_string(i) + "].Quadratic", PointLights[i]->Quadratic);
			sceneShader->SetFloat("pointLights[" + std::to_string(i) + "].Radius", PointLights[i]->Radius);
			sceneShader->SetFloat("pointLights[" + std::to_string(i) + "].Intensity", PointLights[i]->Intensity);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST);

		sceneShader->SetInt32("Output", static_cast<uint32_t>(RenderOutputImage));
	}


	void RenderAPI::StartTextRenderPass() {
		if (textShader == nullptr || textShader->ProgramID == 0) {
			Logger::Log(LogMessageLevels::BAD_ERROR, "StartTextRenderPass: textShader es no ha sido cargado.", __LINE__);
			return;
		}
		if (window == nullptr) {
			Logger::Log(LogMessageLevels::BAD_ERROR, "StartTextRenderPass: ventana es nullptr.", __LINE__);
			return;
		}

		textShader->Use();
		textShader->SetProjection(glm::ortho(0.0f, static_cast<GLfloat>(window->ScreenSizeX), 0.0f, static_cast<GLfloat>(window->ScreenSizeY)));
		glActiveTexture(GL_TEXTURE0);

		glDisable(GL_DEPTH_TEST);
	}


	void RenderAPI::StartSpriteRenderPass() {
		if (spriteShader == nullptr || spriteShader->ProgramID == 0) {
			Logger::Log(LogMessageLevels::BAD_ERROR, "spriteShader no ha sido cargado.", __LINE__);
			return;
		}
		if (window == nullptr) {
			Logger::Log(LogMessageLevels::BAD_ERROR, "StartSpriteRenderPass: ventana es nullptr.", __LINE__);
			return;
		}

		spriteShader->Use();
		spriteShader->SetProjection(glm::ortho(0.0f, static_cast<GLfloat>(window->ScreenSizeX), 0.0f, static_cast<GLfloat>(window->ScreenSizeY)));
		glActiveTexture(GL_TEXTURE0);

		glDisable(GL_DEPTH_TEST);
	}


	void RenderAPI::StartSecondRenderPass() {
		if (screenShader == nullptr || screenShader->ProgramID == 0) {
			Logger::Log(LogMessageLevels::BAD_ERROR, "screenShader no ha sido cargado.", __LINE__);
			return;
		}
		if (window == nullptr) {
			Logger::Log(LogMessageLevels::BAD_ERROR, "StartSecondRenderPass: ventana es nullptr.", __LINE__);
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		screenShader->Use();
		screenShader->SetInt32("screenTexture", 0);
		screenShader->SetInt32("ScreenSizeX", window->ScreenSizeX);
		screenShader->SetInt32("ScreenSizeY", window->ScreenSizeY);
	}


	void RenderAPI::DrawSkybox() {
		if (skyboxShader == nullptr || skyboxShader->ProgramID == 0) {
			Logger::Log(LogMessageLevels::BAD_ERROR, "skyboxShader no ha sido cargado.", __LINE__);
			return;
		}
		if (!skybox.IsLoaded) {
			Logger::Log(LogMessageLevels::BAD_ERROR, "skybox no ha sido cargado.", __LINE__);
			return;
		}

		glDepthFunc(GL_LEQUAL);

		skyboxShader->Use();
		skyboxShader->SetView(glm::mat4(glm::mat3(glm::lookAt(camera->CameraTransform.GlobalPosition.ToGLM(), (camera->CameraTransform.GlobalPosition + camera->Front).ToGLM(), camera->Up.ToGLM()))));
		skyboxShader->SetProjection(glm::perspective(glm::radians(static_cast<float_t>(camera->FieldOfView)), window->ScreenRatio, 0.1f, 100.0f));
		
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.ID);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
	}


	void RenderAPI::RenderScreen() {
		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderTexture.ID);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}


	void RenderAPI::AddPointLight(PointLight* luz) {
		if (PointLights.size() < MAX_LIGHTS) {
			PointLights.push_back(luz);
		}
		else {
			Logger::Log(LogMessageLevels::WARNING, "ya hay " + std::to_string(PointLights.size()) + " luces puntuales.");
		}
	}


	void RenderAPI::enableOrDisableSetting(const RenderSettings& settings, const bool& value) {

		switch (settings) {
		case RenderSettings::ENABLE_BLEND:
			EnableBlend = value;
			if (value)
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);
			break;
		case RenderSettings::ENABLE_CULL_FACE:
			EnableCullFace = value;
			if (value)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
			break;
		case RenderSettings::ENABLE_DEPTH_TEST:
			EnableDepthTest = value;
			if (value)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
			break;
		case RenderSettings::ENABLE_STENCIL_TEST:
			EnableStencilTest = value;
			if (value)
				glEnable(GL_STENCIL_TEST);
			else
				glDisable(GL_STENCIL_TEST);
			break;
		case RenderSettings::DYNAMIC_RES:
			DynamicResolution = value;
			UpdateViewport();
			break;
		case RenderSettings::V_SYNC:
			VerticalSync = true;
			break;
		default:
			break;
		}
	}


	void RenderAPI::UpdateViewport() {
		glViewport(0, 0, window->ScreenSizeX, window->ScreenSizeY);

		if (DynamicResolution) {
			glBindTexture(GL_TEXTURE_2D, renderTexture.ID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window->ScreenSizeX, window->ScreenSizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture.ID, 0);

			glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObject);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window->ScreenSizeX, window->ScreenSizeY);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Logger::Log(LogMessageLevels::BAD_ERROR, "FRAMEBUFFER: Framebuffer is not complete", __LINE__);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		glfwSwapInterval(static_cast<int32_t>(VerticalSync));
	}


	void RenderAPI::drawUIElement(UI::BaseUIElement* element) {
		/*if (element == nullptr || !element->IsActive || !element->IsVisible)
			return;

		if (element->ImplementedInterfacesBitFlags & UIInterfacesBitFlags::UIHasTexture) {
			IUserInterfaceHasTexture* var = dynamic_cast<IUserInterfaceHasTexture*>(element);
			
			if (var->Image != nullptr)
				DrawTexture(*var->Image, Vector2(element->Rectangle.X, element->Rectangle.Y), Vector2(element->Rectangle.GetRectangleWidth(), element->Rectangle.GetRectangleHeight()), var->TextureColor * element->Opacity);
		}

		if (element->ImplementedInterfacesBitFlags & UIInterfacesBitFlags::UIHasText) {
			spriteShader->SetBool("isText", 1);
			IUserInterfaceHasText* var = dynamic_cast<IUserInterfaceHasText*>(element);
			
			if (var->TextFont != nullptr)
				DrawString(*var->TextFont, var->TextSize, var->Text, Vector2(0.0f), var->TextColor * element->Opacity, var->TextAnchor, Vector4(element->Rectangle.X + var->TextBorderLimit, element->Rectangle.Y + var->TextBorderLimit, element->Rectangle.Z - var->TextBorderLimit, element->Rectangle.W - var->TextBorderLimit));
			
			spriteShader->SetBool("isText", 0);
		}

		for (auto i : element->Children)
			drawUIElement(i);*/
	}


	Vector2 RenderAPI::getTextPosition(const Vector2& position, const Vector2& textSize, const Anchor& anchor, const AnchorTextTo& to, const Vector4& reference) const {
		Vector4 refr = Vector4(0.0f, 0.0f, window->ScreenSizeX, window->ScreenSizeY);

		if (to == AnchorTextTo::UI_ELEMENT)
			refr = reference;

		switch (anchor) {
			case Anchor::BOTTOM_LEFT:
				return Vector2(refr.X + position.X, refr.Y + position.Y);

			case Anchor::BOTTOM_RIGHT:
				return Vector2(refr.X + position.X + refr.GetRectangleWidth() - textSize.X, refr.Y + position.Y);

			case Anchor::TOP_LEFT:
				return Vector2(refr.X + position.X, refr.Y - position.Y + refr.GetRectangleHeight() - textSize.Y);

			case Anchor::TOP_RIGHT:
				return Vector2(refr.X + position.X + refr.GetRectangleWidth() - textSize.X, refr.Y - position.Y + refr.GetRectangleHeight() - textSize.Y);

			case Anchor::CENTER:
				return Vector2(refr.X + position.X + refr.GetRectangleWidth() / 2 - textSize.X / 2, refr.Y + position.Y + refr.GetRectangleHeight() / 2 - textSize.Y / 2);

			default:
				return Vector2(refr.X + position.X, refr.Y + position.Y);
		}
	}

}