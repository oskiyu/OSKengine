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
	/// - "sceneImage": imágen de entrada.
	/// - "finalImage": imágen de salida.
	class OSKAPI_CALL IPostProcessPass {

	public:

		/// @brief Define cómo va a acceder el efecto
		/// de post-procesado a su imágen de entrada.
		/// 
		/// @deprecated En un futuro se eliminará la posibilidad de elegir
		/// el tipo de acceso, y los efectos de post-procesado
		/// deberán acceder a su imagen de entrada como un SAMPLER.
		enum class InputType {
			STORAGE_IMAGE,
			SAMPLER
		};


		virtual ~IPostProcessPass() = default;

		
		/// @brief Crea el render target de salida.
		/// Se puede sobreescribir por efectos de post-procesado específicos
		/// para crear recursos intermedios.
		/// @param size Resolución (tanto de entrada como de salida).
		virtual void Create(const Vector2ui& size);

		/// <summary>
		/// Cambia de tamaño el render target de salida.
		/// Se puede sobreescribir para crear recursos intermedios.
		/// </summary>
		/// <param name="size">Tamaño de la imagen final.</param>
		virtual void Resize(const Vector2ui& size);


		/// <summary>
		/// Establece las imágenes de entrada a partir de las que
		/// se calculará el efecto.
		/// </summary>
		/// <param name="images">Imágenes de entrada.</param>
		/// 
		/// @warning Si las imágenes son invalidadas (porque su dueño
		/// original se recrean o cambian de tamaño), se debe volver a 
		/// establecer como imágenes de entrada con IPostProcessPass::SetInput.
		void SetInput(GpuImage* images[3], const GpuImageViewConfig& viewConfig);

		/// @brief Establece las imágenes de entrada a partir de las que
		/// se calculará el efecto.
		/// @param target Render target de entrada.
		/// 
		/// @warning Si las imágenes son invalidadas (porque su dueño
		/// original se recrean o cambian de tamaño), se debe volver a 
		/// establecer como imágenes de entrada con IPostProcessPass::SetInput.
		void SetInput(const RenderTarget& target, const GpuImageViewConfig& viewConfig, InputType type = InputType::STORAGE_IMAGE);
		
		/// @brief Establece las imágenes de entrada a partir de las que
		/// se calculará el efecto.
		/// @param target Render target de entrada.
		/// 
		/// @warning Si las imágenes son invalidadas (porque su dueño
		/// original se recrean o cambian de tamaño), se debe volver a 
		/// establecer como imágenes de entrada con IPostProcessPass::SetInput.
		void SetInput(const RtRenderTarget& target, const GpuImageViewConfig& viewConfig, InputType type = InputType::STORAGE_IMAGE);


		/// <summary>
		/// Genera la imagen final.
		/// </summary>
		/// <param name="computeCmdList">Lista de comandos de computación.</param>
		/// 
		/// @pre Se deben haber establecido las imágenes de entrada con IPostProcessPass::SetInput.
		/// @pre Las imágenes de entrada establecidas con IPostProcessPass::SetInput deben estar
		/// en un estado válido.
		/// @pre computeCmdList debe ser una lista de comandos de computación.
		virtual void Execute(ICommandList* computeCmdList) = 0;


		/// <summary>
		/// Devuelve el render target con la imagen final
		/// en las imágenes de color.
		/// </summary>
		const ComputeRenderTarget& GetOutput() const;


		/// <summary>
		/// Actualiza la instancia de material.
		/// Se debe llamar después de haber creado/recreado
		/// el pass y después de haber establecido las imágenes de entrada.
		/// </summary>
		/// 
		/// @pre Debe haberse creado el pase con IPostProcessPass::Create.
		/// @pre Debe llamarse desapués de cambiar de tamaño el pase con IPostProcessPass::Resize.
		/// @pre Las imágenes de entrada deben haberse establecido con IPostProcessPass::SetInput.
		/// @pre Si las imágenes de entrada han sido invalidadas, se deben establecer de nuevo con IPostProcessPass::SetInput.
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
