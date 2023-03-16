#pragma once

#include "IPostProcessPass.h"

namespace OSK::GRAPHICS {

	class ICommandList;
	class IGpuStorageBuffer;

	/// <summary>
	/// Clase auxiliar que permite una capa de bloom / resplandor.
	/// 
	/// Partiendo de una imagen de entrada (preferiblemente
	/// con un formato HDR) genera una nueva imagen con un resplandor
	/// alrededor de las zonas más brillantes de la imagen de entrada.
	/// 
	/// La imagen final contiene la imagen original + resplandor.
	/// 
	/// Usa shaders de computación para ejecutar un algoritmo basado en: 
	/// http://www.iryoku.com/next-generation-post-processing-in-call-of-duty-advanced-warfare
	/// </summary>
	class OSKAPI_CALL BloomPass : public IPostProcessPass {

	public:

		void Create(const Vector2ui& size) override;
		void Resize(const Vector2ui& size) override;

		void Execute(ICommandList* computeCmdList) override;

		void UpdateMaterialInstance() override;

	private:

		/// <summary>
		/// Establece las imágenes intermedias de bloom en el
		/// material instance.
		/// </summary>
		void SetupMaterialInstances();


		/// @brief Ejecuta un escalado o desescalado de un nivel de mip a otro.
		/// @param computeCmdList Lista de comandos donde se ejecutará.
		/// @param sourceMipLevel Nivel de mip de origen.
		/// @param destMipLevel Nivel de mip que se escribirá.
		/// 
		/// @pre computeCmdList debe ser una lista de comandos de computación. 
		void ExecuteSinglePass(
			ICommandList* computeCmdList, 
			TIndex sourceMipLevel,
			TIndex destMipLevel);
		
		/// <summary>
		/// Procesa el proceso de desescalado completo.
		/// </summary>
		/// <param name="computeCmdList">Lista de comandos donde se ejecutará.</param>
		/// <param name="res">Puntero a un vector 2D que contiene la resolución del pase actual.</param>
		/// 
		/// @pre computeCmdList debe ser una lista de comandos de computación. 
		/// 
		/// @post *res será la resolución después de haberse realizado todo el desescalado.
		void DownscaleBloom(ICommandList* computeCmdList);

		/// <summary>
		/// Procesa el proceso de escalado completo.
		/// </summary>
		/// <param name="computeCmdList">Lista de comandos donde se ejecutará.</param>
		/// <param name="res">Puntero a un vector 2D que contiene la resolución del pase actual.</param>
		/// 
		/// @pre computeCmdList debe ser una lista de comandos de computación. 
		/// 
		/// @post *res será la resolución después de haberse realizado todo el escalado.
		void UpscaleBloom(ICommandList* computeCmdList);

		/// @brief Número máximo de pases de downscale/upscale.
		const static TSize maxNumPasses = 8;


		/// @brief Instancias de los materiales de downscale.
		/// Hay una instancia por cada paso de downscale, configurada
		/// con los mip-maps necesistados por cada paso.
		UniquePtr<MaterialInstance> downscalingMaterialInstance[maxNumPasses]{};

		/// @brief Instancias de los materiales de upscale.
		/// Hay una instancia por cada paso de upscale, configurada
		/// con los mip-maps necesistados por cada paso.
		/// 
		/// @note Aunque tiene una instancia para el nivel 0, esta no se debe usar:
		/// se debe usar @p resolveInstance.
		UniquePtr<MaterialInstance> upscalingMaterialInstance[maxNumPasses]{};

		/// @brief Instancia del material final.
		UniquePtr<MaterialInstance> resolveInstance;

		Material* downscaleMaterial = nullptr;
		Material* upscaleMaterial = nullptr;
		Material* resolveMaterial = nullptr;

		/// @brief Calcula el número de pases, de acuerdo al número
		/// de mip-levels del render target.
		/// @return Número de pases de downscale/upscale.
		TSize GetNumPasses() const;

	};

}
