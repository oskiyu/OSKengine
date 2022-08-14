#pragma once

#include "IGpuImage.h"
#include "UniquePtr.hpp"
#include "MaterialInstance.h"
#include "Vector2.hpp"
#include "RenderTarget.h"


namespace OSK::GRAPHICS {

	class ICommandList;

	/// <summary>
	/// Clase auxiliar que permite una capa de bloom / resplandor.
	/// 
	/// Partiendo de una imagen de entrada (preferiblemente
	/// con un formato HDR) genera una nueva imagen con un resplandor
	/// alrededor de las zonas más brillantes de la imagen de entrada.
	/// 
	/// La imagen final únicamente consisitira del resplandor, siendo el resto
	/// de la imagen transparente.
	/// 
	/// Usa shaders de computación para ejecutar un algoritmo basado en: 
	/// http://www.iryoku.com/next-generation-post-processing-in-call-of-duty-advanced-warfare
	/// </summary>
	class BloomPass {

	public:

		/// <summary>
		/// El constructor carga el material de bloom
		/// y crea las instancias del material.
		/// </summary>
		/// 
		/// @pre El renderizador está activo.
		BloomPass();


		/// <summary>
		/// Crea las imágenes intermedias de resplandor y
		/// el render target final.
		/// </summary>
		/// <param name="size">Tamaño de la imagen final.</param>
		void Create(const Vector2ui& size);

		/// <summary>
		/// Cambia de tamaño las imágenes intermedias de resplandor y
		/// el render target final.
		/// </summary>
		/// <param name="size">Tamaño de la imagen final.</param>
		void Resize(const Vector2ui& size);


		/// <summary>
		/// Establece las imágenes de entrada a partir de las que
		/// se calculará el bloom.
		/// </summary>
		/// <param name="images">Imágenes de entrada.</param>
		/// 
		/// @warning Si las imágenes son invalidadas (porque su dueño
		/// original se recrean o cambian de tamaño), se debe volver a 
		/// establecer como imágenes de entrada con BloomPass::SetInput.
		void SetInput(const GpuImage* images[3]);

		/// <summary>
		/// Establece las imágenes de entrada a partir de las que
		/// se calculará el bloom.
		/// </summary>
		/// <param name="target">Render target de entrada.</param>
		/// 
		/// @warning Si las imágenes son invalidadas (porque su dueño
		/// original se recrean o cambian de tamaño), se debe volver a 
		/// establecer como imágenes de entrada con BloomPass::SetInput.
		void SetInput(const RenderTarget& target);


		/// <summary>
		/// Genera la imagen de resplandor.
		/// </summary>
		/// <param name="computeCmdList">Lista de comandos de computación.</param>
		/// 
		/// @pre Se deben haber establecido las imágenes de entrada con BloomPass::SetInput.
		/// @pre Las imágenes de entrada establecidas con BloomPass::SetInput deben estar
		/// en un estado válido.
		/// @pre computeCmdList debe ser una lista de comandos de computación.
		void Execute(ICommandList* computeCmdList);


		/// <summary>
		/// Devuelve el render target con la imagen de bloom
		/// renderizada en las imágenes de color.
		/// </summary>
		const RenderTarget& GetRenderTarget() const;

	private:

		/// <summary>
		/// Establece las imágenes intermedias de bloom en el
		/// material instance.
		/// </summary>
		void SetupMaterialInstances();


		/// <summary>
		/// Ejecuta un escalado o desescalado de una imagen intermedia a otra.
		/// </summary>
		/// <param name="computeCmdList">Lista de comandos donde se ejecutará.</param>
		/// <param name="oldRes">Resolución del pase anterior.</param>
		/// <param name="newRes">Resolución de este pase.</param>
		/// <param name="inputIndex">Índice de la imagen anterior. 
		/// Si es 2, es la imagen de entrada original.</param>
		/// <param name="outputIndex">Índice de la imagen donde se ejecutará este pase.</param>
		/// 
		/// @pre computeCmdList debe ser una lista de comandos de computación. 
		/// 
		/// @pre oldRes es menor o igual que la resolución del bloom.
		/// @pre newRes es menor o igual que la resolución del bloom.
		/// 
		/// @pre inputIndex está entre 0 y 2, ambos incluidos.
		/// @pre outputIndex está entre 0 y 1, ambos incluidos.
		/// 
		/// @note Si inputIndex es 2, es la imagen de entrada original.
		void ExecuteSinglePass(ICommandList* computeCmdList, const Vector2f& oldRes, const Vector2f& newRes, TSize inputIndex, TSize outputIndex);
		
		/// <summary>
		/// Procesa el proceso de desescalado completo.
		/// </summary>
		/// <param name="computeCmdList">Lista de comandos donde se ejecutará.</param>
		/// <param name="res">Puntero a un vector 2D que contiene la resolución del pase actual.</param>
		/// 
		/// @pre computeCmdList debe ser una lista de comandos de computación. 
		/// 
		/// @post *res será la resolución después de haberse realizado todo el desescalado.
		void DownscaleBloom(ICommandList* computeCmdList, Vector2f* res);

		/// <summary>
		/// Procesa el proceso de escalado completo.
		/// </summary>
		/// <param name="computeCmdList">Lista de comandos donde se ejecutará.</param>
		/// <param name="res">Puntero a un vector 2D que contiene la resolución del pase actual.</param>
		/// 
		/// @pre computeCmdList debe ser una lista de comandos de computación. 
		/// 
		/// @post *res será la resolución después de haberse realizado todo el escalado.
		void UpscaleBloom(ICommandList* computeCmdList, Vector2f* res);

		UniquePtr<GpuImage> bloomTargets[3]{};

		Material* bloomMaterial = nullptr;
		UniquePtr<MaterialInstance> bloomMaterialInstances[3]{}; // 2 : original
		
		RenderTarget resolveRenderTarget{};

	};

}
