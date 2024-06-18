#pragma once

#include "ApiCall.h"
#include "Vector2.hpp"
#include "RtRenderTarget.h"


namespace OSK::GRAPHICS {

	class ICommandList;


	/// @brief Clase base que permite ejecutar pases de efectos de postprocesamiento.
	/// 
	/// Su instancia de material contiene un slot "texture" con dos entradas:
	/// - "sceneImage": im�gen de entrada.
	/// - "finalImage": im�gen de salida.
	class OSKAPI_CALL IPostProcessPass {

	public:

		virtual ~IPostProcessPass() = default;

		
		/// @brief Crea el render target de salida.
		/// Se puede sobreescribir por efectos de post-procesado espec�ficos
		/// para crear recursos intermedios.
		/// @param size Resoluci�n (tanto de entrada como de salida).
		virtual void Create(const Vector2ui& size);

		/// @brief Cambia de tama�o el render target de salida.
		/// Se puede sobreescribir para crear recursos intermedios.
		/// @param size Tama�o de la imagen final.
		virtual void Resize(const Vector2ui& size);


		/// @brief Genera la imagen final.
		/// @param computeCmdList Lista de comandos sobre la que se ejecutar�.
		/// 
		/// @pre Se deben haber establecido las im�genes de entrada con `IPostProcessPass::SetInput`.
		/// @pre Las im�genes de entrada establecidas con `IPostProcessPass::SetInput` deben estar
		/// en un estado v�lido.
		/// @pre @p computeCmdList debe soportar, al menos .
		/// 
		/// @pre Se deben haber establecido las entradas necesarias
		/// para el pase.
		/// @throws PostProcessInputNotSetException si no se han establecido
		/// las entradas necesarias.
		virtual void Execute(ICommandList* computeCmdList) = 0;


		/// @return Render target con la imagen final
		/// en las im�genes de color.
		ComputeRenderTarget& GetOutput();

		/// @return Render target con la imagen final
		/// en las im�genes de color.
		const ComputeRenderTarget& GetOutput() const;

	private:

		ComputeRenderTarget m_resolveRenderTarget{};

	};

}
