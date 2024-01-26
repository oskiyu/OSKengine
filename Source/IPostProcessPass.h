#pragma once

#include "UniquePtr.hpp"
#include "MaterialInstance.h"
#include "Vector2.hpp"
#include "RtRenderTarget.h"

#include <array>
#include <span>

namespace OSK::GRAPHICS {

	class ICommandList;
	class RenderTarget;


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


		/// @brief Establece las im�genes de entrada a partir de las que
		/// se calcular� el efecto.
		/// @param image Imagen de entrada.
		/// @param viewConfig 
		/// 
		/// @warning Si las im�genes son invalidadas (porque su due�o
		/// original se recrean o cambian de tama�o), se debe volver a 
		/// establecer como im�genes de entrada con IPostProcessPass::SetInput.
		virtual void SetInput(
			GpuImage* image,
			const GpuImageViewConfig& viewConfig);

		/// @brief Establece las im�genes de entrada a partir de las que
		/// se calcular� el efecto.
		/// @param target Render target de entrada.
		/// 
		/// @warning Si las im�genes son invalidadas (porque su due�o
		/// original se recrean o cambian de tama�o), se debe volver a 
		/// establecer como im�genes de entrada con IPostProcessPass::SetInput.
		void SetInputTarget(RenderTarget& target, const GpuImageViewConfig& viewConfig);
		
		/// @brief Establece las im�genes de entrada a partir de las que
		/// se calcular� el efecto.
		/// @param target Render target de entrada.
		/// 
		/// @warning Si las im�genes son invalidadas (porque su due�o
		/// original se recrean o cambian de tama�o), se debe volver a 
		/// establecer como im�genes de entrada con IPostProcessPass::SetInput.
		void SetInputTarget(RtRenderTarget& target, const GpuImageViewConfig& viewConfig);


		/// <summary>
		/// Genera la imagen final.
		/// </summary>
		/// <param name="computeCmdList">Lista de comandos de computaci�n.</param>
		/// 
		/// @pre Se deben haber establecido las im�genes de entrada con IPostProcessPass::SetInput.
		/// @pre Las im�genes de entrada establecidas con IPostProcessPass::SetInput deben estar
		/// en un estado v�lido.
		/// @pre computeCmdList debe ser una lista de comandos de computaci�n.
		virtual void Execute(ICommandList* computeCmdList) = 0;


		/// <summary>
		/// Devuelve el render target con la imagen final
		/// en las im�genes de color.
		/// </summary>
		ComputeRenderTarget& GetOutput();

		/// <summary>
		/// Devuelve el render target con la imagen final
		/// en las im�genes de color.
		/// </summary>
		const ComputeRenderTarget& GetOutput() const;


		/// <summary>
		/// Actualiza la instancia de material.
		/// Se debe llamar despu�s de haber creado/recreado
		/// el pass y despu�s de haber establecido las im�genes de entrada.
		/// </summary>
		/// 
		/// @pre Debe haberse creado el pase con IPostProcessPass::Create.
		/// @pre Debe llamarse desapu�s de cambiar de tama�o el pase con IPostProcessPass::Resize.
		/// @pre Las im�genes de entrada deben haberse establecido con IPostProcessPass::SetInput.
		/// @pre Si las im�genes de entrada han sido invalidadas, se deben establecer de nuevo con IPostProcessPass::SetInput.
		virtual void UpdateMaterialInstance();

	protected:

		void SetupDefaultMaterialInstances();

		Material* postProcessingMaterial = nullptr;
		UniquePtr<MaterialInstance> postProcessingMaterialInstance;

		ComputeRenderTarget resolveRenderTarget{};

		GpuImage* inputImage = nullptr;
		const IGpuImageView* inputView = nullptr;

	};

}
