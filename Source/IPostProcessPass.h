#pragma once

#include "ApiCall.h"
#include "Vector2.hpp"
#include "RtRenderTarget.h"


namespace OSK::GRAPHICS {

	class ICommandList;


	/// @brief Clase base que permite ejecutar pases de efectos de postprocesamiento.
	/// 
	/// Su instancia de material contiene un slot "texture" con dos entradas:
	/// - "sceneImage": imágen de entrada.
	/// - "finalImage": imágen de salida.
	class OSKAPI_CALL IPostProcessPass {

	public:

		virtual ~IPostProcessPass() = default;

		
		/// @brief Crea el render target de salida.
		/// Se puede sobreescribir por efectos de post-procesado específicos
		/// para crear recursos intermedios.
		/// @param size Resolución (tanto de entrada como de salida).
		virtual void Create(const Vector2ui& size);

		/// @brief Cambia de tamaño el render target de salida.
		/// Se puede sobreescribir para crear recursos intermedios.
		/// @param size Tamaño de la imagen final.
		virtual void Resize(const Vector2ui& size);


		/// @brief Genera la imagen final.
		/// @param computeCmdList Lista de comandos sobre la que se ejecutará.
		/// 
		/// @pre Se deben haber establecido las imágenes de entrada con `IPostProcessPass::SetInput`.
		/// @pre Las imágenes de entrada establecidas con `IPostProcessPass::SetInput` deben estar
		/// en un estado válido.
		/// @pre @p computeCmdList debe soportar, al menos .
		/// 
		/// @pre Se deben haber establecido las entradas necesarias
		/// para el pase.
		/// @throws PostProcessInputNotSetException si no se han establecido
		/// las entradas necesarias.
		virtual void Execute(ICommandList* computeCmdList) = 0;


		/// @return Render target con la imagen final
		/// en las imágenes de color.
		ComputeRenderTarget& GetOutput();

		/// @return Render target con la imagen final
		/// en las imágenes de color.
		const ComputeRenderTarget& GetOutput() const;

	private:

		ComputeRenderTarget m_resolveRenderTarget{};

	};

}
