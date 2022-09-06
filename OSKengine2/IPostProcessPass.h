#pragma once

#include "UniquePtr.hpp"
#include "MaterialInstance.h"
#include "Vector2.hpp"
#include "RenderTarget.h"

namespace OSK::GRAPHICS {

	class ICommandList;

	/// <summary>
	/// Clase base que permite ejecutar pases de efectos de postprocesamiento.
	/// 
	/// Su instancia de material contiene un slot "texture" con dos entradas:
	/// - "sceneImage": imágen de entrada.
	/// - "finalImage": imágen de salida.
	/// </summary>
	class OSKAPI_CALL IPostProcessPass {

	public:

		enum class InputType {
			STORAGE_IMAGE,
			SAMPLER
		};


		virtual ~IPostProcessPass() = default;

		/// <summary>
		/// Crea el render target de salida.
		/// Se puede sobreescribir para crear recursos intermedios.
		/// </summary>
		/// <param name="size">Tamaño de la imagen final.</param>
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
		void SetInput(GpuImage* images[3], InputType type = InputType::STORAGE_IMAGE);

		/// <summary>
		/// Establece las imágenes de entrada a partir de las que
		/// se calculará el efecto.
		/// </summary>
		/// <param name="target">Render target de entrada.</param>
		/// 
		/// @warning Si las imágenes son invalidadas (porque su dueño
		/// original se recrean o cambian de tamaño), se debe volver a 
		/// establecer como imágenes de entrada con IPostProcessPass::SetInput.
		void SetInput(const RenderTarget& target, InputType type = InputType::STORAGE_IMAGE);


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
		const RenderTarget& GetOutput() const;


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

		RenderTarget resolveRenderTarget{};

		GpuImage* inputImages[3]{};

	};

}
