#pragma once

#include "UniquePtr.hpp"
#include "MaterialInstance.h"
#include "Vector2.hpp"
#include "RtRenderTarget.h"

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

		/// @brief Define c�mo va a acceder el efecto
		/// de post-procesado a su im�gen de entrada.
		/// 
		/// @deprecated En un futuro se eliminar� la posibilidad de elegir
		/// el tipo de acceso, y los efectos de post-procesado
		/// deber�n acceder a su imagen de entrada como un SAMPLER.
		enum class InputType {
			STORAGE_IMAGE,
			SAMPLER
		};


		virtual ~IPostProcessPass() = default;

		
		/// @brief Crea el render target de salida.
		/// Se puede sobreescribir por efectos de post-procesado espec�ficos
		/// para crear recursos intermedios.
		/// @param size Resoluci�n (tanto de entrada como de salida).
		virtual void Create(const Vector2ui& size);

		/// <summary>
		/// Cambia de tama�o el render target de salida.
		/// Se puede sobreescribir para crear recursos intermedios.
		/// </summary>
		/// <param name="size">Tama�o de la imagen final.</param>
		virtual void Resize(const Vector2ui& size);


		/// <summary>
		/// Establece las im�genes de entrada a partir de las que
		/// se calcular� el efecto.
		/// </summary>
		/// <param name="images">Im�genes de entrada.</param>
		/// 
		/// @warning Si las im�genes son invalidadas (porque su due�o
		/// original se recrean o cambian de tama�o), se debe volver a 
		/// establecer como im�genes de entrada con IPostProcessPass::SetInput.
		void SetInput(GpuImage* images[3], const GpuImageViewConfig& viewConfig);

		/// @brief Establece las im�genes de entrada a partir de las que
		/// se calcular� el efecto.
		/// @param target Render target de entrada.
		/// 
		/// @warning Si las im�genes son invalidadas (porque su due�o
		/// original se recrean o cambian de tama�o), se debe volver a 
		/// establecer como im�genes de entrada con IPostProcessPass::SetInput.
		void SetInput(const RenderTarget& target, const GpuImageViewConfig& viewConfig, InputType type = InputType::STORAGE_IMAGE);
		
		/// @brief Establece las im�genes de entrada a partir de las que
		/// se calcular� el efecto.
		/// @param target Render target de entrada.
		/// 
		/// @warning Si las im�genes son invalidadas (porque su due�o
		/// original se recrean o cambian de tama�o), se debe volver a 
		/// establecer como im�genes de entrada con IPostProcessPass::SetInput.
		void SetInput(const RtRenderTarget& target, const GpuImageViewConfig& viewConfig, InputType type = InputType::STORAGE_IMAGE);


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

		GpuImage* inputImages[NUM_RESOURCES_IN_FLIGHT]{};
		const IGpuImageView* inputViews[NUM_RESOURCES_IN_FLIGHT]{};

	};

}
