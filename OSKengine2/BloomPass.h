#pragma once

#include "IPostProcessPass.h"

namespace OSK::GRAPHICS {

	class ICommandList;

	/// <summary>
	/// Clase auxiliar que permite una capa de bloom / resplandor.
	/// 
	/// Partiendo de una imagen de entrada (preferiblemente
	/// con un formato HDR) genera una nueva imagen con un resplandor
	/// alrededor de las zonas m�s brillantes de la imagen de entrada.
	/// 
	/// La imagen final �nicamente consisitira del resplandor, siendo el resto
	/// de la imagen transparente.
	/// 
	/// Usa shaders de computaci�n para ejecutar un algoritmo basado en: 
	/// http://www.iryoku.com/next-generation-post-processing-in-call-of-duty-advanced-warfare
	/// </summary>
	class OSKAPI_CALL BloomPass : public IPostProcessPass {

	public:

		void Create(const Vector2ui& size) override;
		void Resize(const Vector2ui& size) override;

		void Execute(ICommandList* computeCmdList) override;

		void SetExposureBuffers(const IGpuStorageBuffer* [3]);

		void UpdateMaterialInstance() override;

	private:

		/// <summary>
		/// Establece las im�genes intermedias de bloom en el
		/// material instance.
		/// </summary>
		void SetupMaterialInstances();


		/// <summary>
		/// Ejecuta un escalado o desescalado de una imagen intermedia a otra.
		/// </summary>
		/// <param name="computeCmdList">Lista de comandos donde se ejecutar�.</param>
		/// <param name="oldRes">Resoluci�n del pase anterior.</param>
		/// <param name="newRes">Resoluci�n de este pase.</param>
		/// <param name="inputIndex">�ndice de la imagen anterior. 
		/// Si es 2, es la imagen de entrada original.</param>
		/// <param name="outputIndex">�ndice de la imagen donde se ejecutar� este pase.</param>
		/// 
		/// @pre computeCmdList debe ser una lista de comandos de computaci�n. 
		/// 
		/// @pre oldRes es menor o igual que la resoluci�n del bloom.
		/// @pre newRes es menor o igual que la resoluci�n del bloom.
		/// 
		/// @pre inputIndex est� entre 0 y 2, ambos incluidos.
		/// @pre outputIndex est� entre 0 y 1, ambos incluidos.
		/// 
		/// @note Si inputIndex es 2, es la imagen de entrada original.
		void ExecuteSinglePass(ICommandList* computeCmdList, const Vector2f& oldRes, const Vector2f& newRes, TSize inputIndex, TSize outputIndex);
		
		/// <summary>
		/// Procesa el proceso de desescalado completo.
		/// </summary>
		/// <param name="computeCmdList">Lista de comandos donde se ejecutar�.</param>
		/// <param name="res">Puntero a un vector 2D que contiene la resoluci�n del pase actual.</param>
		/// 
		/// @pre computeCmdList debe ser una lista de comandos de computaci�n. 
		/// 
		/// @post *res ser� la resoluci�n despu�s de haberse realizado todo el desescalado.
		void DownscaleBloom(ICommandList* computeCmdList, Vector2f* res);

		/// <summary>
		/// Procesa el proceso de escalado completo.
		/// </summary>
		/// <param name="computeCmdList">Lista de comandos donde se ejecutar�.</param>
		/// <param name="res">Puntero a un vector 2D que contiene la resoluci�n del pase actual.</param>
		/// 
		/// @pre computeCmdList debe ser una lista de comandos de computaci�n. 
		/// 
		/// @post *res ser� la resoluci�n despu�s de haberse realizado todo el escalado.
		void UpscaleBloom(ICommandList* computeCmdList, Vector2f* res);

		UniquePtr<GpuImage> bloomTargets[3]{};

		Material* downscaleMaterial = nullptr;
		Material* upscaleMaterial = nullptr;
		Material* finalMaterial = nullptr;

		const static TSize numPasses = 4;

	};

}
